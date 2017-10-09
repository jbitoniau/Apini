var dgram = require('dgram');

/*
    TelemetryReceiver
*/
var TelemetryReceiver = function() {

    this._localPort = 8092;
    this._remotePort = 8082;

    this._udpSocket = dgram.createSocket('udp4');
    this._udpSocket.on(
        'listening',
        function() {
            var address = this._udpSocket.address();
            console.log('TelemetryReceiver: opened UDP socket on ' + address.address + ':' + address.port);
        }.bind(this)
    );
    this._udpSocket.bind(this._localPort, '127.0.0.1');

    this._onTelemetrySampleReadyListeners = [];

    this._onUDPSocketMessageHandler = this._onUDPSocketMessage.bind(this);
    this._udpSocket.on('message', this._onUDPSocketMessageHandler);
};

TelemetryReceiver.prototype.dispose = function() {
    console.log('DISPOSE TelemetryReceiver!');
    this._udpSocket.removeListener('message', this._onUDPSocketMessageHandler);

    // CLOSE SOCKET!
};

TelemetryReceiver.prototype._onUDPSocketMessage = function(message, remote) {
    var uint8Array = new Uint8Array(message);

    if (remote.port !== this._remotePort) {
        console.warn('TelemetryReceiver: receiving data from unexpected remove port');
    }

    // var text = 'UDP socket received message on ' + remote.address + ':' + remote.port;
    // var messageAsHex = '';
    // for (var i = 0; i < uint8Array.length /* same as remote.size */; i++) {
    //     messageAsHex += uint8Array[i].toString(16) + ' ';
    // }
    // text += ' - ' + messageAsHex;
    //console.log(text);

    var numMotors = 4;

    var telemetrySample = {
        timestamp: 0,

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

        throttle: 0,
        rudder: 0,
        elevators: 0,
        ailerons: 0,
    };

    for ( var i=0; i<numMotors; i++ ) {
        telemetrySample["motorPowerLevel"+i.toString()] = 0;
    }
    for ( var i=0; i<numMotors; i++ ) {
        telemetrySample["motorPulseWidth"+i.toString()] = 0;
    }


    var dataView = new DataView(uint8Array.buffer);
    var offset = 0;
    telemetrySample.timestamp = dataView.getUint32(offset, true);
    offset += 4;

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

    telemetrySample.throttle = dataView.getFloat32(offset, true);
    offset += 4;
    telemetrySample.rudder = dataView.getFloat32(offset, true);
    offset += 4;
    telemetrySample.elevators = dataView.getFloat32(offset, true);
    offset += 4;
    telemetrySample.ailerons = dataView.getFloat32(offset, true);
    offset += 4;

    for ( var i=0; i<numMotors; i++ ) {
        telemetrySample["motorPowerLevel"+i.toString()] = dataView.getFloat32(offset, true);
        offset += 4;
    }  
    for ( var i=0; i<numMotors; i++ ) {
        telemetrySample["motorPulseWidth"+i.toString()] = dataView.getUint32(offset, true);
        offset += 4;
    }

    for (var i = 0; i < this._onTelemetrySampleReadyListeners.length; ++i) {
        var listener = this._onTelemetrySampleReadyListeners[i];
        listener(telemetrySample);
    }
};

exports.TelemetryReceiver = TelemetryReceiver;
