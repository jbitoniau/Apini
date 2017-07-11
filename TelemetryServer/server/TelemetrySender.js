/*
    TelemetrySender

    A TelemetrySender pairs a SensorReader to a websocket connection.

    Whenever a data point is ready from the SensorReader, this object stores it. 
    Then on a regular basisc, it sends the data points accumulated on the 
    websocket. This basically data points to be send as a group and not individually
    which would be costly.
*/
var TelemetrySender = function(telemetryReceiver, websocketConnection) {
    this._telemetryReceiver = telemetryReceiver;
    this._websocketConnection = websocketConnection;

    this._interval = setInterval(this._sendData.bind(this), 50);

    this._maxNumTelemetrySamplesToSend = 2000;
    this._telemetrySamplesToSend = [];

    this._onTelemetrySampleReadyHandler = this._onTelemetrySampleReady.bind(this);
    this._telemetryReceiver._onTelemetrySampleReadyListeners.push(this._onTelemetrySampleReadyHandler);

    this._instanceID = TelemetrySender.numInstances; // Just for debug
    TelemetrySender.numInstances++;

    console.log('TelemetrySender #' + this._instanceID + ': created');
};
TelemetrySender.numInstances = 0;

TelemetrySender.prototype.dispose = function() {
    console.log('TelemetrySender#' + this._instanceID + ': dispose');

    clearInterval(this._interval);

    var index = this._telemetryReceiver._onTelemetrySampleReadyListeners.indexOf(this._onSensorDataReadyHandler);
    if (index !== -1) {
        this._telemetryReceiver._onTelemetrySampleReadyListeners.splice(index, 1);
        TelemetrySender.numInstances--;
    } else {
        console.error("something's wrong");
    }
};

TelemetrySender.prototype._onTelemetrySampleReady = function(telemetrySample) {
    this._telemetrySamplesToSend.splice(0, 0, telemetrySample);
    if (this._telemetrySamplesToSend.length > this._maxNumTelemetrySamplesToSend) {
        this._telemetrySamplesToSend.shift();
        console.warn('buffer full');
    }
};

TelemetrySender.prototype._sendData = function() {
    //console.log("TelemetrySender#"+ this._instanceID + ": sending " + this._telemetrySamplesToSend.length);
    var jsonData = JSON.stringify(this._telemetrySamplesToSend);
    this._websocketConnection.sendUTF(jsonData);
    this._telemetrySamplesToSend = [];
};

exports.TelemetrySender = TelemetrySender;
