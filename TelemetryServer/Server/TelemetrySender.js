/*
    TelemetrySender
*/
var TelemetrySender = function(websocketConnection, updateIntervalMs) {
    this._websocketConnection = websocketConnection;

    this._interval = setInterval(this.send.bind(this), updateIntervalMs);

    this._maxNumTelemetrySamplesToSend = 100;
    this._telemetrySamplesToSend = [];

    // this._onTelemetrySampleReadyHandler = this._onTelemetrySampleReady.bind(this);
    // this._telemetryReceiver._onTelemetrySampleReadyListeners.push(this._onTelemetrySampleReadyHandler);

    this.thisWebsocketProvidesFlightControls = false;

    this._instanceID = TelemetrySender.numInstances; // Just for debug
    TelemetrySender.numInstances++;

    console.log('TelemetrySender #' + this._instanceID + ': created');
};
TelemetrySender.numInstances = 0;

TelemetrySender.prototype.dispose = function() {
    console.log('TelemetrySender #' + this._instanceID + ': dispose');
    clearInterval(this._interval);
    TelemetrySender.numInstances--;
};

TelemetrySender.prototype.addTelemetrySampleToSend = function(telemetrySample) {
    // TODO: check state of websocketConnection here...
    var sample = {};
    Object.assign( sample, telemetrySample );
    sample.thisWebsocketProvidesFlightControls = this.thisWebsocketProvidesFlightControls;
    this._telemetrySamplesToSend.splice(0, 0, sample);
    if (this._telemetrySamplesToSend.length > this._maxNumTelemetrySamplesToSend) {
        this._telemetrySamplesToSend.shift();
        console.warn('TelemetrySender.addTelemetrySampleToSend: buffer is full, dropping oldest telemetry sample');
    }
};

TelemetrySender.prototype.send = function() {
    //console.log("TelemetrySender#"+ this._instanceID + ": sending " + this._telemetrySamplesToSend.length );
    var jsonData = JSON.stringify(this._telemetrySamplesToSend);
    this._websocketConnection.sendUTF(jsonData);
    this._telemetrySamplesToSend = [];
};

exports.TelemetrySender = TelemetrySender;
