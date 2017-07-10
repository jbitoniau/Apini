var SensorReaderUDP = function(udpSocket) {
    this._udpSocket = udpSocket;
    this._onSensorDataReadyListeners = [];

    this._onUDPSocketMessageHandler = this._onUDPSocketMessage.bind(this);
    this._udpSocket.on('message', this._onUDPSocketMessageHandler);
}

SensorReaderUDP.prototype.dispose = function() {
    console.log('DISPOSE SENSORREADERUDP!');
    this._udpSocket.removeListener('message', this._onUDPSocketMessageHandler);
};

SensorReaderUDP.prototype._onUDPSocketMessage = function(message, remote) {
    var text = 'UDP socket received message on ' + remote.address + ':' + remote.port;
    var uint8Array = new Uint8Array(message);
    var messageAsHex = '';
    for (var i = 0; i < uint8Array.length /* same as remote.size */; i++) messageAsHex += uint8Array[i].toString(16) + ' ';
    text += ' - ' + messageAsHex;
    //console.log(text);

    var dataPoint = {
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
    dataPoint.accelerationX = dataView.getFloat64(offset, true);
    offset += 8;
    dataPoint.accelerationY = dataView.getFloat64(offset, true);
    offset += 8;
    dataPoint.accelerationZ = dataView.getFloat64(offset, true);
    offset += 8;
    dataPoint.angularSpeedX = dataView.getFloat64(offset, true);
    offset += 8;
    dataPoint.angularSpeedY = dataView.getFloat64(offset, true);
    offset += 8;
    dataPoint.angularSpeedZ = dataView.getFloat64(offset, true);
    offset += 8;
    dataPoint.temperature = dataView.getFloat32(offset, true);
    offset += 4;

    dataPoint.magneticHeadingX = dataView.getFloat64(offset, true);
    offset += 8;
    dataPoint.magneticHeadingY = dataView.getFloat64(offset, true);
    offset += 8;
    dataPoint.magneticHeadingZ = dataView.getFloat64(offset, true);
    offset += 8;

    dataPoint.temperature2 = dataView.getFloat32(offset, true);
    offset += 4;
    dataPoint.pressure = dataView.getFloat32(offset, true);
    offset += 4;

    dataPoint.timestamp = dataView.getUint32(offset, true);
    offset += 4;

    for (var i = 0; i < this._onSensorDataReadyListeners.length; ++i) {
        var listener = this._onSensorDataReadyListeners[i];
        listener(dataPoint);
    }
};

exports.SensorReaderUDP = SensorReaderUDP;
