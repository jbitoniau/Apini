/*
    FlightControlsReceiver
*/
var FlightControlsReceiver = function(websocketConnection) {
    this._websocketConnection = websocketConnection;
    this._websocketConnection.on('message', function(message) {
        //console.log('FlightControlsReceiver: Received ' + message.type + ' message:' + message.utf8Data );
        var flightControls = JSON.parse(message.utf8Data);
        if ( this._onFlightControlsReceived  )
        {
            this._onFlightControlsReceived( flightControls );
        }
    }.bind(this));

    this._onFlightControlsReceived = null;
};

exports.FlightControlsReceiver = FlightControlsReceiver;