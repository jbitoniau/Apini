var FlightControls = require('./Common/FlightControls').FlightControls;

/*
    FlightControlsReceiver
*/
var FlightControlsReceiver = function(websocketConnection) {
    this._websocketConnection = websocketConnection;
    this._onWebsocketConnectionMessageHandler = this._onWebsocketConnectionMessage.bind(this);
    this._websocketConnection.on('message', this._onWebsocketConnectionMessageHandler);
    this._onFlightControlsReceived = null;
};

FlightControlsReceiver.prototype.dispose = function() {
    console.log("FlightControlsReceiver.prototype.dispose!!!!!!");
    this._websocketConnection.removeListener('message', this._onWebsocketConnectionMessageHandler);
};

FlightControlsReceiver.prototype._onWebsocketConnectionMessage = function(message) {
   console.log('FlightControlsReceiver: Received ' + message.type + ' message:' + message.utf8Data );
    var jsonObject = JSON.parse(message.utf8Data);

    var flightControls = new FlightControls();
    flightControls.copy( jsonObject );      // The values in the raw JSON object are copied into the FlightControls object with proper validation
    
    if (this._onFlightControlsReceived) {
        this._onFlightControlsReceived(flightControls);
    }
};

exports.FlightControlsReceiver = FlightControlsReceiver;
