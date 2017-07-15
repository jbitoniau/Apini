'use strict';

function TelemetryReceiver( websocket ) {
    this._websocket = websocket;
    this._onSocketMessageHandler = this._onSocketMessage.bind(this);
    this._websocket.addEventListener('message', this._onSocketMessageHandler);
    this.onTelemetrySamplesReceived = null;
}

TelemetryReceiver.prototype.dispose = function() {
    this._websocket.removeEventListener('message', this._onSocketMessageHandler);
};

TelemetryReceiver.prototype._onSocketMessage = function( message ) {
    if ( this.onTelemetrySamplesReceived ) {
        var telemetrySamples = JSON.parse(message.data);
        this.onTelemetrySamplesReceived(telemetrySamples);
    }
};