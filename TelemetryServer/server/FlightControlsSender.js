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
    console.log('FlightControlsSender.dispose: to do!');
    // CLOSE SOCKET!
};

FlightControlsSender.prototype.send = function(flightControls) {
    
    var uint8Array = new Uint8Array(512);
    var dataView = new DataView(uint8Array.buffer);

    var offset = 0;
    dataView.setFloat32(offset, flightControls.throttle, true);     // true for little endian!!! 
    offset+=4;
    dataView.setFloat32(offset, flightControls.rudder, true); 
    offset+=4;

    // var txt = "";
    // for ( var i=0; i<offset; i++ )
    // {
    //     txt += uint8Array[i].toString(16);// + "-";
    // }
    // console.log('FlightControlsSender: to ' + this._remoteAddress + ':' + this._remotePort + ": " + txt );
    
    var buffer = Buffer.from(uint8Array);       // There must be ways to avoid this copy here...
    this._udpSocket.send(buffer, 0, offset, this._remotePort, this._remoteAddress, function(err, bytes) {
        if (err) {
            console.warn('FlightControlsSender: error while sending data');
        }
        // close?
    });
};

exports.FlightControlsSender = FlightControlsSender;
