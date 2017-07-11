var dgram = require('dgram');

/*
    TelemetryReceiver
*/
var TelemetryReceiver = function() {
    this._udpSocket = dgram.createSocket('udp4');
    this._udpSocket.on('listening', function() {
        var address = this._udpSocket.address();
        console.log('UDP Server listening on ' + address.address + ':' + address.port);
    }.bind(this));
    this._udpSocket.bind(8181, '127.0.0.1');

    this._onTelemetrySampleReadyListeners = [];

    this._onUDPSocketMessageHandler = this._onUDPSocketMessage.bind(this);
    this._udpSocket.on('message', this._onUDPSocketMessageHandler);
}

TelemetryReceiver.prototype.dispose = function() {
    console.log('DISPOSE TelemetryReceiver!');
    this._udpSocket.removeListener('message', this._onUDPSocketMessageHandler);

    // CLOSE SOCKET!
};

TelemetryReceiver.prototype._onUDPSocketMessage = function(message, remote) {
    var uint8Array = new Uint8Array(message);
    
    // var text = 'UDP socket received message on ' + remote.address + ':' + remote.port;
    // var messageAsHex = '';
    // for (var i = 0; i < uint8Array.length /* same as remote.size */; i++) {
    //     messageAsHex += uint8Array[i].toString(16) + ' ';
    // }
    // text += ' - ' + messageAsHex;
    //console.log(text);

    var telemetrySample = {
        accelerationX: 0,
        accelerationY: 0,
        accelerationZ: 0,
        angularSpeedX: 0,
        angularSpeedY: 0,
        angularSpeedZ: 0,
        temperature: 0,

        magneticHeadingX: 0,
        magneticHeadingY: 0,
        magneticHeadingZ: 0,

        temperature2: 0,
        pressure: 0,

        timestamp: 0
    };

    var dataView = new DataView(uint8Array.buffer);
    var offset = 0;
    telemetrySample.accelerationX = dataView.getFloat64(offset, true);
    offset += 8;
    telemetrySample.accelerationY = dataView.getFloat64(offset, true);
    offset += 8;
    telemetrySample.accelerationZ = dataView.getFloat64(offset, true);
    offset += 8;
    telemetrySample.angularSpeedX = dataView.getFloat64(offset, true);
    offset += 8;
    telemetrySample.angularSpeedY = dataView.getFloat64(offset, true);
    offset += 8;
    telemetrySample.angularSpeedZ = dataView.getFloat64(offset, true);
    offset += 8;
    telemetrySample.temperature = dataView.getFloat32(offset, true);
    offset += 4;

    telemetrySample.magneticHeadingX = dataView.getFloat64(offset, true);
    offset += 8;
    telemetrySample.magneticHeadingY = dataView.getFloat64(offset, true);
    offset += 8;
    telemetrySample.magneticHeadingZ = dataView.getFloat64(offset, true);
    offset += 8;

    telemetrySample.temperature2 = dataView.getFloat32(offset, true);
    offset += 4;
    telemetrySample.pressure = dataView.getFloat32(offset, true);
    offset += 4;

    telemetrySample.timestamp = dataView.getUint32(offset, true);
    offset += 4;

    for (var i = 0; i < this._onTelemetrySampleReadyListeners.length; ++i) {
        var listener = this._onTelemetrySampleReadyListeners[i];
        listener(telemetrySample);
    }
};

exports.TelemetryReceiver = TelemetryReceiver;
