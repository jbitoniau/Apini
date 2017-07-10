'use strict';

function Main() {
	var canvas = document.getElementById('graphCanvas');
	canvas.focus();

	var telemetryViewer = new TelemetryViewer(canvas);

	// Data types
	var dataTypes = Object.keys(telemetryViewer._graphDataWindows);
	var dataTypeSelect = document.getElementById('dataTypeSelect');
	for (var i = 0; i < dataTypes.length; i++) {
		var dataType = dataTypes[i];
		var option = new Option(dataType, dataType);
		dataTypeSelect.options[i] = option;
	}
	dataTypeSelect.addEventListener('change', function(event) {
		var graphDataType = event.target.value;
		telemetryViewer.setGraphDataType(graphDataType);
	});
	var currentGraphDataType = telemetryViewer.getGraphDataType();
	dataTypeSelect.value = currentGraphDataType;

	// Fullscreen
	var fullscreenButton = document.getElementById('fullscreenButton');
	if (window.screenfull && screenfull.enabled) {
		document.addEventListener(screenfull.raw.fullscreenchange, function(event) {
			if (screenfull.isFullscreen) fullscreenButton.className = 'roundedButtonToggled';
			else fullscreenButton.className = 'roundedButton';
		});

		fullscreenButton.onclick = function(event) {
			if (screenfull && screenfull.enabled) {
				var mainDiv = document.getElementById('mainDiv');
				screenfull.toggle(mainDiv);
			}
		};
	} else {
		fullscreenButton.style.display = 'none';
	}

	// Autoscroll
	var autoscrollButton = document.getElementById('autoscrollButton');
	autoscrollButton.onclick = function(event) {
		var autoscroll = !telemetryViewer.getAutoscroll();
		telemetryViewer.setAutoscroll(autoscroll);
	};
	telemetryViewer._onAutoscrollChanged = function() {
		var autoscroll = telemetryViewer.getAutoscroll();
		if (autoscroll) autoscrollButton.className = 'roundedButtonToggled';
		else autoscrollButton.className = 'roundedButton';
	};
	telemetryViewer._onAutoscrollChanged();

	// Connection indicator
	var connectionIndicator = document.getElementById('connectionIndicator');
	telemetryViewer._onConnectionOpen = function() {
		connectionIndicator.style.backgroundColor = 'green';
	};
	telemetryViewer._onConnectionError = function() {
		connectionIndicator.style.backgroundColor = 'red';
	};
	telemetryViewer._onConnectionClose = function() {
		connectionIndicator.style.backgroundColor = 'grey';
	};
}
