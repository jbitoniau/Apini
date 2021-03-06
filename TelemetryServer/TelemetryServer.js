'use strict';

var http = require('http');
var https = require('https');
var url = require('url');
//var querystring = require('querystring');
var fs = require('fs');
var websocket = require('websocket'); // don't forget to run "npm install websocket"

var TelemetryReceiver = require('./Server/TelemetryReceiver').TelemetryReceiver;
var TelemetrySender = require('./Server/TelemetrySender').TelemetrySender;
var FlightControlsReceiver = require('./Server/FlightControlsReceiver').FlightControlsReceiver;
var FlightControlsSender = require('./Server/FlightControlsSender').FlightControlsSender;

/*
    TelemetryServer
*/
function TelemetryServer() {

    // Create TelemetryReceiver working on the UDP socket
    this._telemetryReceiver = new TelemetryReceiver();

    // Prepare TelemetrySenders array for incoming websocket connections
    this._telemetrySenders = [];

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

    // Flight controls receiver and sender
    this._flightControlsReceiver = null;
    this._flightControlsSender = new FlightControlsSender();

    // Create Websocket server
    this._websocketServer = new websocket.server({
        httpServer: this._httpServer
    });

    this._websocketServer.on(
        'request',
        function(request) {
            var websocketConnection = request.accept(null, request.origin);

            // Create a new TelemetrySender for this websocket connection
            var telemetrySender = new TelemetrySender(websocketConnection, 50);
            
            // TODO: this needs to be less hacky...
            if (!this._flightControlsReceiver) {
                console.log('TelemetryServer: using this new connection to create FlightControlsReceiver');
                this._flightControlsReceiver = new FlightControlsReceiver(websocketConnection);
                this._flightControlsReceiver._onFlightControlsReceived = function(flightControls) {
                    this._flightControlsSender.send(flightControls);
                    //console.log("p:" + flightControls.pTerm + " i:" + flightControls.iTerm + " d:" + flightControls.dTerm );
                }.bind(this);
                telemetrySender.thisWebsocketProvidesFlightControls = true;
            }

            // Whenever we get a new telemetry sample from the TelemetryReceiver, give it to the TelemetrySender for sending
            var onTelemetrySampleReadyHandler = function(telemetrySample) {
                telemetrySender.addTelemetrySampleToSend(telemetrySample);
            };
            this._telemetryReceiver._onTelemetrySampleReadyListeners.push(onTelemetrySampleReadyHandler);

            // Remember this TelemetrySender
            this._telemetrySenders.push(telemetrySender);

            console.log('TelemetryServer: ' + this._telemetrySenders.length + ' connections in progress. thisWebsocketProvidesFlightControls:' + telemetrySender.thisWebsocketProvidesFlightControls);

            var telemetryServer = this;
            websocketConnection.on(
                'close',
                function(reasonCode, description) {
                    //console.log("reasonCode: " + reasonCode + " desc:" + description);

                    // TODO: this needs to be less hacky...
                    if (telemetryServer._flightControlsReceiver && websocketConnection === telemetryServer._flightControlsReceiver._websocketConnection) {
                        console.log('TelemetryServer: no more FlightControlsReceiver');
                        telemetryServer._flightControlsReceiver.dispose();
                        telemetryServer._flightControlsReceiver = null;
                    }

                    // Stop forwarding telemetry samples from TelemetryReceiver to TelemetrySender
                    var index = telemetryServer._telemetryReceiver._onTelemetrySampleReadyListeners.indexOf(onTelemetrySampleReadyHandler);
                    if (index !== -1) {
                        telemetryServer._telemetryReceiver._onTelemetrySampleReadyListeners.splice(index, 1);
                    } else {
                        console.warn("TelemetryServer: couldn't find TelemetrySender's handler in TelemetryReceiver's listeners");
                    }

                    // Forget about this TelemetrySender
                    var index = telemetryServer._telemetrySenders.indexOf(telemetrySender);
                    if (index !== -1) {
                        telemetryServer._telemetrySenders.splice(index, 1);
                    } else {
                        console.warn("TelemetryServer: couldn't find TelemetrySender corresponding to closing connection");
                    }

                    // Dispose TelemetrySender
                    telemetrySender.dispose();

                    console.log('TelemetryServer: ' + telemetryServer._telemetrySenders.length + ' connections in progress');
                }
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

    filename = 'Client/' + filename;

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
            '       <p>' +
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
