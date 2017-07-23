'use strict';

function TelemetryViewer(graphCanvas, flightControlsCanvas) {
    this._graphCanvas = graphCanvas;
    this._flightControlsCanvas = flightControlsCanvas;

    // Current and backed-up graph data windows for each type of data
    var now = new Date().getTime();
    var initialWidth = 10 * 1000;
    var initialX = now - initialWidth / 2;
    this._graphDataWindow = { x: initialX, y: -5, width: initialWidth, height: 40 };
    this._graphDataWindows = {
        acceleration: { x: initialX, y: -4, width: initialWidth, height: 8 },
        angularSpeed: { x: initialX, y: -1000, width: initialWidth, height: 2000 },
        magneticHeading: { x: initialX, y: -750, width: initialWidth, height: 1500 },
        temperature: { x: initialX, y: -5, width: initialWidth, height: 40 },
        pressure: { x: initialX, y: 1005, width: initialWidth, height: 20 },
        flightControls: { x: initialX, y: -0.7, width: initialWidth, height: 1.9 },
        motorPulseWidth: { x: initialX, y: 950000, width: initialWidth, height: 1100000 }
    };

    this._graphDataTypeOptions = {
        acceleration: {
            yPropertyName: ['accelerationX', 'accelerationY', 'accelerationZ'],
            colors: {
                dataLine: ['#990000', '#009900', '#000099'],
                dataPoint: ['#990000', '#009900', '#000099']
            }
        },
        angularSpeed: {
            yPropertyName: ['angularSpeedX', 'angularSpeedY', 'angularSpeedZ'],
            colors: {
                dataLine: ['#990000', '#009900', '#000099'],
                dataPoint: ['#990000', '#009900', '#000099']
            }
        },
        magneticHeading: {
            yPropertyName: ['magneticHeadingX', 'magneticHeadingY', 'magneticHeadingZ'],
            colors: {
                dataLine: ['#990000', '#009900', '#000099'],
                dataPoint: ['#990000', '#009900', '#000099']
            }
        },
        temperature: {
            yPropertyName: ['temperature', 'temperature2'],
            colors: {
                dataLine: ['#990000', '#009900'],
                dataPoint: ['#990000', '#009900']
            }
        },
        pressure: {
            yPropertyName: 'pressure',
            colors: {
                dataLine: '#990000',
                dataPoint: '#990000'
            }
        },
        flightControls: {
            yPropertyName: ['throttle', 'rudder', 'elevators', 'ailerons'],
            colors: {
                dataLine: ['#990000', '#009900', '#000099', '#009999'],
                dataPoint: ['#990000', '#009900', '#000099', '#009999']
            }
        },
        motorPulseWidth: {
            yPropertyName: ['pulseWidthMotor0', 'pulseWidthMotor1', 'pulseWidthMotor2', 'pulseWidthMotor3'],
            colors: {
                dataLine: ['#990000', '#009900', '#000099', '#009999'],
                dataPoint: ['#990000', '#009900', '#000099', '#009999']
            }
        }
    };

    this._graphData = [];
    this._maxNumGraphDataPoints = null; 
    if ( isMobileDevice() ) {
        this._maxNumGraphDataPoints = 20 * 1000/20;
    }
    else {
        this._maxNumGraphDataPoints = 1 * 60 * 1000/20;  
    }

    this._graphOptions = {
        yPropertyName: null, // initialization takes place later
        clearCanvas: true,
        drawOriginAxes: true,
        drawDataRange: true,
        drawDataGaps: true,
        contiguityThreshold: 25,
        textSize: 12,
        numMaxLinesX: 5,
        numMaxLinesY: 5,
        getPrimaryLinesTextX: GraphDataPresenter.getLinesTextForTime,
        getPrimaryLinesSpacingX: GraphDataPresenter.getPrimaryLinesSpacingForTime,
        getSecondaryLinesSpacingX: GraphDataPresenter.getSecondaryLinesSpacingForTime,
        getPrimaryLinesTextY: GraphDataPresenter.getLinesText,
        getPrimaryLinesSpacingY: GraphDataPresenter.getLinesSpacing,
        getSecondaryLinesSpacingY: GraphDataPresenter.getSecondaryLinesSpacing,
        points: {
            //typicalDataPointXSpacing: 10*60*1000,     // No need if we provide a contiguityThreshold
            maxPointSize: 5,
            maxNumPoints: 500
        },
        colors: {
            /*    clear:'#FFFFFF',
            dataRange: "#EEEEEE",
            dataGaps: "#EEEEEE",
            axesLines: "#AA6666",
            primaryLinesText: '#AA6666',
            primaryLines: '#FFAAAA',
            secondaryLines: '#FFDDDD',*/
            dataLine: '#884444',
            dataPoint: '#884444'
        }
    };

    // The graph controller is responsible for rendering the graph and handling input events to navigate in it
    this._graphController = new GraphController(this._graphCanvas, this._graphData, this._graphDataWindow, this._graphOptions);

    // When the user navigates in the graph (i.e. changes the graph data window), we need to check whether more data needs to be fetched
    this._graphController._onGraphDataWindowChange = this._onGraphDataWindowChange.bind(this);
    this._autoscroll = true;

    // The type of graph data currently being displayed
    this._graphDataType = null;
    this.setGraphDataType('flightControls');

    // Flight controls
    this._flightControlsProvider = new FlightControlsProvider();
    this._flightControlsIntervalPeriod = 20; // In milliseconds
    this._flightControlsInterval = null;
    this._debugFakeFlightControls = false;

    // Data transmitter objects
    this._flightControlsSender = null;
    this._telemetryReceiver = null;
    this._onTelemetrySamplesReceivedHandler = this._onTelemetrySamplesReceived.bind(this);

    // Websocket
    this._isConnected = false;
    this._onSocketOpenHandler = this._onSocketOpen.bind(this);
    this._onSocketErrorHandler = this._onSocketError.bind(this);
    this._onSocketCloseHandler = this._onSocketClose.bind(this);
    this._openWebsocket();

    this._onWindowResizeHandler = this._onWindowResize.bind(this);
    window.addEventListener('resize', this._onWindowResizeHandler);
    this._onWindowResize();

    // Events
    this.onGraphDataTypeChanged = null;
    this.onAutoscrollChanged = null;
    this.onConnectionOpen = null;
    this.onConnectionError = null;
    this.onConnectionClose = null;
}

TelemetryViewer.prototype.dispose = function() {
    this._closeWebsocket();
};

TelemetryViewer.prototype._openWebsocket = function() {
    if (this._websocket) {
        throw new Error('Invalid state');
    }
    var host = window.location.host;
    this._websocket = new WebSocket('ws://' + host);
    this._websocket.addEventListener('open', this._onSocketOpenHandler);
    this._websocket.addEventListener('error', this._onSocketErrorHandler);
    this._websocket.addEventListener('close', this._onSocketCloseHandler);
};

TelemetryViewer.prototype._closeWebsocket = function() {
    if (this._websocket === null) {
        throw new Error('Invalid state');
    }

    clearInterval(this._flightControlsInterval);
    this._flightControlsSender.dispose();
    this._flightControlsSender = null;
    this._flightControlsProvider.dispose();
    this._flightControlsProvider = null;
    this._flightControlsSender.dispose();
    this._flightControlsSender = null;

    this._telemetryReceiver.dispose();
    this._telemetryReceiver = null;

    this._websocket.removeEventListener('open', this._onSocketOpenHandler);
    this._websocket.removeEventListener('error', this._onSocketErrorHandler);
    this._websocket.removeEventListener('close', this._onSocketCloseHandler);
    this._websocket.close();
    this._websocket = null;
};

TelemetryViewer.prototype._onSocketOpen = function(/*??*/) {
    this._isConnected = true;

    // FlightControlsSender
    this._flightControlsSender = new FlightControlsSender(this._websocket);
    this._flightControlsInterval = setInterval(
        function() {
            var flightControls = null;
            this._flightControlsProvider.update();
            if (this._flightControlsProvider.isGamepadConnected()) {
                flightControls = this._flightControlsProvider.flightControls;
            } else {
                flightControls = new FlightControls();
                if ( this._debugFakeFlightControls ) {
                    var now = performance.now();
                    var t = Math.floor(now) % 1000 / 1000;
                    flightControls.throttle = Math.sin(Math.PI * 2 * t) / 2 + 0.5;
                    t = Math.floor(now) % 2300 / 2300;
                    flightControls.rudder = Math.sin(Math.PI * 2 * t) / 2;
                    t = Math.floor(now) % 1100 / 1100;
                    flightControls.elevators = Math.sin(Math.PI * 2 * t) / 2;
                    t = Math.floor(now) % 5000 / 5000;
                    flightControls.ailerons = Math.sin(Math.PI * 2 * t) / 2;
                }
            }
            this._flightControlsSender.send(flightControls);
        }.bind(this),
        this._flightControlsIntervalPeriod
    );

    // TelemetryReceiver
    this._telemetryReceiver = new TelemetryReceiver(this._websocket);
    this._telemetryReceiver.onTelemetrySamplesReceived = this._onTelemetrySamplesReceivedHandler;

    if (this.onConnectionOpen) {
        this.onConnectionOpen();
    }
};

TelemetryViewer.prototype._onSocketError = function(error) {
    if (this.onConnectionError) {
        this.onConnectionError();
    }
    alert('WebSocket error: ' + error);
};

TelemetryViewer.prototype._onSocketClose = function(/*??*/) {
    this._isConnected = false;
    if (this.onConnectionClose) {
        this.onConnectionClose();
    }
};

TelemetryViewer.prototype._onTelemetrySamplesReceived = function(telemetrySamples) {
    // Add the samples to the beginning of the graph data array
    // The grapher draws most recent samples at beginning of array first
    for (var i = 0; i < telemetrySamples.length; i++) {
        var telemetrySample = telemetrySamples[i];
        telemetrySample.x = telemetrySample.timestamp; // The grapher requires an 'x' property
        this._graphData.splice(0, 0, telemetrySample);
    }

    // If there's a maximum number of samples to hold, enforce it
    if (typeof this._maxNumGraphDataPoints === 'number') {
        var numExcessDataPoints = this._graphData.length - this._maxNumGraphDataPoints;
        if (numExcessDataPoints > 0) {
            this._graphData.splice( -numExcessDataPoints );
        }
    }

    // TODO: move rendering somewhere else
    this._render();
    
    if (telemetrySamples.length > 0 && telemetrySamples[0].thisWebsocketProvidesFlightControls) {
        this._flightControlsCanvas.style.display = 'block';
        var flightControls = this._flightControlsProvider.flightControls;

        var options = null;
        if (!this._flightControlsProvider.isGamepadConnected()) {
            options = {
                stickFillColor: '#EEEEFF',
                stickStrokeColor: '#AAAAFF',
                knobFillColor: '#CCCCFF',
                knobStrokeColor: '#9999FF'
            };
        }
        FlightControlsPresenter.render(this._flightControlsCanvas, flightControls, options);
    } else {
        this._flightControlsCanvas.style.display = 'none';
    }
};

TelemetryViewer.prototype.getAutoscroll = function() {
    return this._autoscroll;
};

TelemetryViewer.prototype.setAutoscroll = function(autoscroll) {
    if (this._autoscroll === autoscroll) {
        return;
    }

    this._autoscroll = autoscroll;

    if (this._autoscroll) {
        this._scrollToLatestData();
        this._render();
    }

    if (this.onAutoscrollChanged) {
        this.onAutoscrollChanged();
    }
};

TelemetryViewer.prototype.getGraphDataType = function() {
    return this._graphDataType;
};

TelemetryViewer.prototype.setGraphDataType = function(graphDataType) {
    if (graphDataType === this._graphDataType) {
        return;
    }
    if (this._graphDataType) {
        // Remember current graph data y/height for current data type
        this._graphDataWindows[this._graphDataType].y = this._graphDataWindow.y;
        this._graphDataWindows[this._graphDataType].height = this._graphDataWindow.height;
    }

    // Change data type
    var prevGraphDataType = this._graphDataType;
    this._graphDataType = graphDataType;
    Object.assign(this._graphOptions, this._graphDataTypeOptions[this._graphDataType]);

    // Restore graph data y/height for new current type of data to display
    this._graphDataWindow.y = this._graphDataWindows[this._graphDataType].y;
    this._graphDataWindow.height = this._graphDataWindows[this._graphDataType].height;

    // Render graph
    this._graphController.render();

    // Notify
    if (this.onGraphDataTypeChanged) {
        this.onGraphDataTypeChanged(prevGraphDataType, this._graphDataType);
    }
};

TelemetryViewer.prototype._render = function() {
    if (this._autoscroll) {
        this._scrollToLatestData();
    }

    GraphDataPresenter.render(this._graphCanvas, this._graphData, this._graphDataWindow, this._graphOptions);
};

TelemetryViewer.prototype._onGraphDataWindowChange = function(prevGraphDataWindow) {
    if (this.getAutoscroll()) {
        if (this._graphDataWindow.x !== prevGraphDataWindow.x) {
            this.setAutoscroll(false);
        }
    }
};

// Change the x position of the graph data window to show the latest data points.
// This method doesn't affect the other graph data window properties.
TelemetryViewer.prototype._scrollToLatestData = function() {
    var graphData = this._graphData;
    if (graphData.length === 0) {
        return;
    }
    var latestDataPoint = graphData[0];
    this._graphDataWindow.x = latestDataPoint.x - this._graphDataWindow.width;
};

TelemetryViewer.prototype._onWindowResize = function(event) {
    var width = window.innerWidth;
    var numPoints = Math.floor(width / 10);
    this._graphOptions.points.maxNumPoints = numPoints;
};


/*
    isMobileDevice
*/
function isMobileDevice()
{
    // http://stackoverflow.com/questions/11381673/detecting-a-mobile-browser
    // http://stackoverflow.com/questions/3514784/what-is-the-best-way-to-detect-a-mobile-device-in-jquery
    // http://detectmobilebrowsers.com/
    if ( navigator.userAgent.match(/Android/i)||
         navigator.userAgent.match(/webOS/i)|| 
         navigator.userAgent.match(/iPhone/i)||
         navigator.userAgent.match(/iPad/i)||
         navigator.userAgent.match(/iPod/i)||
         navigator.userAgent.match(/BlackBerry/i)||
         navigator.userAgent.match(/Windows Phone/i) )
    {
        return true;
    }
    else 
    {
        return false;
    }
}