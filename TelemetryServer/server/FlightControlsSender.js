var dgram = require('dgram');

/*
    FlightControlsSender
*/
var FlightControlsSender = function() {
    this._localPort = 8091;
    this._remotePort = 8081;
    this._remoteAddress = '127.0.0.1';

    this._udpSocket = dgram.createSocket('udp4');
    this._udpSocket.on(
        'listening',
        function() {
            var address = this._udpSocket.address();
            console.log('FlightControlsSender: opened UDP socket on ' + address.address + ':' + address.port);
        }.bind(this)
    );
    this._udpSocket.bind(this._localPort, '127.0.0.1');
};

FlightControlsSender.prototype.dispose = function() {
    console.log('DISPOSE FlightControlsSender!');
    // CLOSE SOCKET!
};

FlightControlsSender.prototype.send = function(flightControls) {
    var message = 'Hello!';
    var buffer = new Buffer(message);

    this._udpSocket.send(buffer, 0, message.length, this._remotePort, this._remoteAddress, function(err, bytes) {
        if (err) {
            console.warn('FlightControlsSender: error while sending data');
        }
        // close?
    });

    console.log('UDP message sent to ' + this._remoteAddress + ':' + this._remotePort);
};

/*var uint8Array = new Uint8Array(message);
    
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
    }*/

exports.FlightControlsSender = FlightControlsSender;
