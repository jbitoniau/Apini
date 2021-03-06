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

function toBuffer(ab) {
    var buf = new Buffer(ab.byteLength);
    var view = new Uint8Array(ab);
    for (var i = 0; i < buf.length; ++i) {
        buf[i] = view[i];
    }
    return buf;
}
FlightControlsSender.prototype.send = function(flightControls) {
    
    var uint8Array = new Uint8Array(512);
    var dataView = new DataView(uint8Array.buffer);
    var offset = 0;
    dataView.setFloat32(offset, flightControls.throttle, true);     // true for little endian!!! 
    offset+=4;
    dataView.setFloat32(offset, flightControls.rudder, true); 
    offset+=4;
    dataView.setFloat32(offset, flightControls.elevators, true); 
    offset+=4;
    dataView.setFloat32(offset, flightControls.ailerons, true); 
    offset+=4;

    dataView.setFloat32(offset, flightControls.pTerm, true); 
    offset+=4;
    dataView.setFloat32(offset, flightControls.iTerm, true); 
    offset+=4;
    dataView.setFloat32(offset, flightControls.dTerm, true); 
    offset+=4;

    var buffer = toBuffer(uint8Array); //Buffer.from(uint8Array);       // There must be ways to avoid this copy here...
    this._udpSocket.send(buffer, 0, offset, this._remotePort, this._remoteAddress, function(err, bytes) {
        if (err) {
            console.warn('FlightControlsSender: error while sending data');
            // close?
        }
    });

    // console.log('FlightControlsSender: to ' + this._remoteAddress + ':' + this._remotePort + ": " +
    //    FlightControlsSender._uint8ArrayAsHexString(uint8Array, offset) + " - " + JSON.stringify(flightControls));
};

FlightControlsSender._uint8ArrayAsHexString = function( uint8Array, length ){
    if ( length===undefined ) {
        length = uint8Array.length;
    }
    var string = "";
    for ( var i=0; i<length; i++ )
    {
        string += uint8Array[i].toString(16);
    }
    return string;
};


exports.FlightControlsSender = FlightControlsSender;
