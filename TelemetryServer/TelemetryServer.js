'use strict';

var http = require('http');
var https = require('https');
var url = require('url');
//var querystring = require('querystring');
var fs = require('fs');
var websocket = require('websocket'); // don't forget to run "npm install websocket"
var dgram = require('dgram');
var sensorReaderUDPMod = require('./server/SensorReaderUDP');
var sensorDataSenderMod = require('./server/SensorDataSender');

/*
	SensorReaderSimulated

	A SensorReader is capable for reading data from a bunch on sensors all at once
	on a regular basis (basically as these data point arrive).

	Whenever this happens, the SensorReader notifies any listener of the new piece of data.
	It doesn't store/remember any data read, this is the responsibility of the code using it.
*/
/*function SensorReaderSimulated() {
	this._interval = setInterval(this._onReceivedData.bind(this), 20);
	this._onSensorDataReadyListeners = [];
}

SensorReaderSimulated.prototype.dispose = function() {
	clearInterval(this._interval);
};

SensorReaderSimulated.prototype._onReceivedData = function() {
	var now = new Date().getTime();
	var t0 = now % 4000 / 4000;
	var t1 = now % 1000 / 1000;
	var t2 = now % 333 / 333;
	var t4 = now % 1642 / 1642;

	var temperature = t0 * 30 - 15 + 50 + Math.sin(t2 * Math.PI * 2) * 2;

	var angularSpeedX =
		Math.sin(t0 * Math.PI * 2) * 15 + Math.sin(t1 * Math.PI * 2) * (5 * (Math.sin(t4 * Math.PI * 2) + 1)) + Math.sin(t2 * Math.PI * 2) * 2 + 50;

	var dataPoint = {
		accelerationX: 10,
		accelerationY: 20,
		accelerationZ: 30,
		angularSpeedX: angularSpeedX,
		angularSpeedY: 15,
		angularSpeedZ: 25,
		temperature: temperature,
		timestamp: new Date().getTime()
	};

	//console.log("SensorReader: read point " + dataPoint.timestamp);

	for (var i = 0; i < this._onSensorDataReadyListeners.length; ++i) {
		var listener = this._onSensorDataReadyListeners[i];
		listener(dataPoint);
	}
};*/

/*
SensorReaderUDP.prototype._onReceivedData = function()
{
	var now = new Date().getTime();
	var t0 = (now % 4000) / 4000;
	var t1 = (now % 1000) / 1000;
	var t2 = (now % 333) / 333;
	var t4 = (now % 1642) / 1642;
	
	var temperature = (t0*30 - 15) + 50  + Math.sin( t2 * Math.PI * 2) * 2;

	var angularSpeedX = 
		Math.sin( t0 * Math.PI * 2) * 15 + 
		Math.sin( t1 * Math.PI * 2) * (5*(Math.sin( t4 * Math.PI * 2)+1)) +
		Math.sin( t2 * Math.PI * 2) * 2 + 
		50;
	
	var dataPoint = {
		temperature: temperature,
		angularSpeedX: angularSpeedX,
		timestamp: new Date().getTime()
	};

	//console.log("SensorReader: read point " + dataPoint.timestamp);

	for ( var i=0; i<this._onSensorDataReadyListeners.length; ++i )
	{
		var listener = this._onSensorDataReadyListeners[i];
		listener( dataPoint );
	}
};*/

/*
	TelemetryServer
*/
function TelemetryServer() {
	// Create UDP socket for discussing with the C++ SensorSender companion program
	var udpSocket = dgram.createSocket('udp4');
	udpSocket.on('listening', function() {
		var address = udpSocket.address();
		console.log('UDP Server listening on ' + address.address + ':' + address.port);
	});
	udpSocket.bind(8181, '127.0.0.1');

	// Create SensorReaderUDP working on the UDP socket
	this._sensorReader = new sensorReaderUDPMod.SensorReaderUDP(udpSocket);

	// Prepare SensorDataSenders array for incoming websocket connections
	this._sensorDataSenders = [];

	// Create HTTP server
	this._httpServer = http.createServer(function(req, res) {
		var path = url.parse(req.url).pathname;
		var query = url.parse(req.url).query;
		if (path.indexOf('/') === 0) {
			var filename = path.substr(1);
			if (filename.length === 0) {
				filename = 'TelemetryViewer.html';
			}
			TelemetryServer._serveFile(filename, res);
		} else {
			TelemetryServer._createHTMLErrorResponse(res, 404, 'Page not found');
		}
	});
	this._httpServer.listen(8080, function() {
		console.log('HTTP server listening');
	});

	// Create Websocket server
	this._websocketServer = new websocket.server({
		httpServer: this._httpServer
	});
	this._websocketServer.on(
		'request',
		function(request) {
			var connection = request.accept(null, request.origin);

			// TEMP! TEMP!
			connection.on('message', function(message) {
				console.log('Received ' + message.type + ' message:' + message.utf8Data);
			});
			// TEMP! TEMP!

			var sensorDataSender = new sensorDataSenderMod.SensorDataSender(this._sensorReader, connection);
			this._sensorDataSenders.push(sensorDataSender);

			console.log('SensorMonitor: ' + this._sensorDataSenders.length + ' connections in progress');

			connection.on(
				'close',
				function(connection) {
					//console.log('Websocket server connection closed' + connection);
					sensorDataSender.dispose();
					var index = this._sensorDataSenders.indexOf(sensorDataSender);
					if (index !== -1) {
						this._sensorDataSenders.splice(index, 1);
					} else {
						console.warn("couldn't find sender for connection...");
					}

					console.log('SensorMonitor: ' + this._sensorDataSenders.length + ' connections in progress');
				}.bind(this)
			);
		}.bind(this)
	);
	console.log('Websocket server created');
}

TelemetryServer.prototype.dispose = function() {
	// Stop things here!
	console.log('dispose to implement here...');
};

TelemetryServer._getFilenameExtension = function(filename) {
	var parts = filename.split('.');
	if (parts.length > 1) {
		return parts[parts.length - 1];
	}
	return '';
};

TelemetryServer._getFileContentType = function(filename) {
	var contentType = null;
	var extension = TelemetryServer._getFilenameExtension(filename).toLowerCase();
	switch (extension) {
		case 'html':
			return 'text/html';
		case 'js':
			return 'application/javascript';
	}
	return null;
};

TelemetryServer._serveFile = function(filename, res) {
	var contentType = TelemetryServer._getFileContentType(filename);
	if (!contentType) {
		console.warn('Serving file: ' + filename + '. Unsupported file/content type');
		res.end();
		return;
	}
	console.log('Serving file: ' + filename + ' as ' + contentType);

	filename = 'client/' + filename;

	fs.readFile(filename, 'utf8', function(err, data) {
		if (err) {
			TelemetryServer._createHTMLErrorResponse(res, 500, err);
		} else {
			res.writeHead(200, { 'content-type': contentType });
			res.write(data);
			res.end();
		}
	});
};

TelemetryServer._createHTMLErrorResponse = function(res, code, message) {
	res.writeHead(code, { 'content-type': 'text/html' });
	res.write(
		'<!DOCTYPE html>' +
			'<html>' +
			'    <head>' +
			'        <meta charset="utf-8" />' +
			'        <title>Error</title>' +
			'    </head>' +
			'    <body>' +
			'     	<p>' +
			message +
			'</p>' +
			'    </body>' +
			'</html>'
	);
	res.end();
};

/*
	Main
*/
function Main() {
	var telemetryServer = new TelemetryServer();

	//http://stackoverflow.com/questions/10021373/what-is-the-windows-equivalent-of-process-onsigint-in-node-js/14861513#14861513
	//http://stackoverflow.com/questions/6958780/quitting-node-js-gracefully
	if (process.platform === 'win32') {
		var rl = require('readline').createInterface({
			input: process.stdin,
			output: process.stdout
		});
		rl.on('SIGINT', function() {
			process.emit('SIGINT');
		});
	}
	process.on('SIGINT', function() {
		console.log('Stopping server...');
		telemetryServer.dispose();
		process.exit();
	});
}

Main();
