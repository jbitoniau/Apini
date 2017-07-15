'use strict';

function TelemetryViewer(canvas) {
    this._canvas = canvas;

    // Current and backed-up graph data windows for each type of data
    var now = new Date().getTime();
    var initialWidth = 10 * 1000;
    var initialX = now - initialWidth / 2;
    this._graphDataWindow = { x: initialX, y: -5, width: initialWidth, height: 40 };
    this._graphDataWindows = {
        accelerationX: { x: initialX, y: -4, width: initialWidth, height: 8 },
        accelerationY: { x: initialX, y: -4, width: initialWidth, height: 8 },
        accelerationZ: { x: initialX, y: -4, width: initialWidth, height: 8 },
        angularSpeedX: { x: initialX, y: -1000, width: initialWidth, height: 2000 },
        angularSpeedY: { x: initialX, y: -1000, width: initialWidth, height: 2000 },
        angularSpeedZ: { x: initialX, y: -1000, width: initialWidth, height: 2000 },
        temperature: { x: initialX, y: -5, width: initialWidth, height: 40 },
        magneticHeadingX: { x: initialX, y: -750, width: initialWidth, height: 1500 },
        magneticHeadingY: { x: initialX, y: -750, width: initialWidth, height: 1500 },
        magneticHeadingZ: { x: initialX, y: -750, width: initialWidth, height: 1500 },
        temperature2: { x: initialX, y: -5, width: initialWidth, height: 40 },
        pressure: { x: initialX, y: 1005, width: initialWidth, height: 20 },
        throttle: { x: initialX, y: -0.2, width: initialWidth, height: 1.4 },
        rudder: { x: initialX, y: -0.7, width: initialWidth, height: 1.4 },
        elevators: { x: initialX, y: -0.7, width: initialWidth, height: 1.4 },
        ailerons: { x: initialX, y: -0.7, width: initialWidth, height: 1.4 }
    };

    this._graphData = [];

    this._graphOptions = {
        yPropertyName: null,    // initialization takes place later
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
        }
        /*colors: {
            clear:'#FFFFFF',
            dataRange: "#EEEEEE",
            dataGaps: "#EEEEEE",
            axesLines: "#AA6666",
            primaryLinesText: '#AA6666',
            primaryLines: '#FFAAAA',
            secondaryLines: '#FFDDDD',
            dataLine: "#884444",
            dataPoint: "#884444",
        },*/
    };

    // The graph controller is responsible for rendering the graph and handling input events to navigate in it
    this._graphController = new GraphController(this._canvas, this._graphData, this._graphDataWindow, this._graphOptions);

    // When the user navigates in the graph (i.e. changes the graph data window), we need to check whether more data needs to be fetched
    this._graphController._onGraphDataWindowChange = this._onGraphDataWindowChange.bind(this);
    this._autoscroll = true;

    // The type of graph data currently being displayed
    this._graphDataType = null;
    this.setGraphDataType('throttle');

    // Flight controls
    this._flightControlsProvider = new FlightControlsProvider();
    this._flightControlsIntervalPeriod = 20;    // In milliseconds
    this._flightControlsInterval = null;

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

    clearInterval( this._flightControlsInterval );
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
            this._flightControlsProvider.update();
            var flightControls = this._flightControlsProvider.flightControls;
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
    for (var i = telemetrySamples.length - 1; i >= 0; i--) {
        var telemetrySample = telemetrySamples[i];
        telemetrySample.x = telemetrySample.timestamp;   // The grapher requires an 'x' property 
        this._graphData.splice(0, 0, telemetrySample);
    }
    this._render();
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
    if ( this._graphDataType ) {
        // Remember current graph data y/height for current data type
        this._graphDataWindows[this._graphDataType].y = this._graphDataWindow.y;
        this._graphDataWindows[this._graphDataType].height = this._graphDataWindow.height;
    }

    // Change data type
    var prevGraphDataType = this._graphDataType;
    this._graphDataType = graphDataType;
    this._graphOptions.yPropertyName = this._graphDataType;

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

    GraphDataPresenter.render(this._canvas, this._graphData, this._graphDataWindow, this._graphOptions);
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
