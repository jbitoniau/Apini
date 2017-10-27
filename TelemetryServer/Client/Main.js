'use strict';

function Main() {
	var graphCanvas = document.getElementById('graphCanvas');
	graphCanvas.focus();
	var flightControlsCanvas = document.getElementById('flightControlsDiv');
	
	var parameterElements = {
		pTermNumberInput: document.getElementById('pTerm'),
		iTermNumberInput: document.getElementById('iTerm'),
		dTermNumberInput: document.getElementById('dTerm')
	};
	var telemetryViewer = new TelemetryViewer(graphCanvas, flightControlsCanvas, parameterElements);

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
	telemetryViewer.onAutoscrollChanged = function() {
		var autoscroll = telemetryViewer.getAutoscroll();
		if (autoscroll) autoscrollButton.className = 'roundedButtonToggled';
		else autoscrollButton.className = 'roundedButton';
	};
	telemetryViewer.onAutoscrollChanged();

	// Connection indicator
	var connectionIndicator = document.getElementById('connectionIndicator');
	telemetryViewer.onConnectionOpen = function() {
		connectionIndicator.style.backgroundColor = 'green';
	};
	telemetryViewer.onConnectionError = function() {
		connectionIndicator.style.backgroundColor = 'red';
	};
	telemetryViewer.onConnectionClose = function() {
		connectionIndicator.style.backgroundColor = 'grey';
	};
}
