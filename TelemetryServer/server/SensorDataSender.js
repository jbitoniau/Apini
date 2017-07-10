

/*
    SensorDataSender

    A SensorDataSender is pairs a SensorReader to a websocket connection.

    Whenever a data point is ready from the SensorReader, this object stores it. 
    Then on a regular basisc, it sends the data points accumulated on the 
    websocket. This basically data points to be send as a group and not individually
    which would be costly.
*/
var SensorDataSender = function(sensorReader, connection) {
    this._sensorReader = sensorReader;
    this._connection = connection;

    this._interval = setInterval(this._sendData.bind(this), 50);

    this._maxNumDataPointsToSend = 2000;
    this._dataPointsToSend = [];

    this._onSensorDataReadyHandler = this._onSensorDataReadyListeners.bind(this);
    this._sensorReader._onSensorDataReadyListeners.push(this._onSensorDataReadyHandler);

    this._instanceID = SensorDataSender.numInstances; // Just for debug
    SensorDataSender.numInstances++;

    console.log('SensorDataSender#' + this._instanceID + ': created');
}
SensorDataSender.numInstances = 0;

SensorDataSender.prototype.dispose = function() {
    clearInterval(this._interval);

    var index = this._sensorReader._onSensorDataReadyListeners.indexOf(this._onSensorDataReadyHandler);
    if (index !== -1) {
        this._sensorReader._onSensorDataReadyListeners.splice(index, 1);
    } else {
        console.error("something's wrong");
    }

    console.log('SensorDataSender#' + this._instanceID + ': dispose');
};

SensorDataSender.prototype._onSensorDataReadyListeners = function(dataPoint) {
    this._dataPointsToSend.splice(0, 0, dataPoint);
    if (this._dataPointsToSend.length > this._maxNumDataPointsToSend) {
        this._dataPointsToSend.shift();
        console.warn('buffer full');
    }
};

SensorDataSender.prototype._sendData = function() {
    //console.log("SensorDataSender#"+ this._instanceID + ": sending " + this._dataPointsToSend.length);
    var jsonData = JSON.stringify(this._dataPointsToSend);
    this._connection.sendUTF(jsonData);
    this._dataPointsToSend = [];
};

exports.SensorDataSender = SensorDataSender;