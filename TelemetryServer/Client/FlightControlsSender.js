'use strict';

function FlightControlsSender(websocket) {
    this._websocket = websocket;
}

FlightControlsSender.prototype.dispose = function() {};

FlightControlsSender.prototype.send = function(flightControls) {
    if (this._websocket.readyState !== 1) {
        console.warn('FlightControlsSender.send: websocket not ready for sending');
        return;
    }

    var jsonString = JSON.stringify(flightControls);
    this._websocket.send(jsonString);
};
