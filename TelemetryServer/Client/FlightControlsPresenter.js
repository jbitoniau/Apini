'use strict';

function FlightControlsPresenter() {}

FlightControlsPresenter.render = function(canvas, flightControls, options) {
    if (canvas.width !== canvas.clientWidth) {
        canvas.width = canvas.clientWidth;
    }
    if (canvas.height !== canvas.clientHeight) {
        canvas.height = canvas.clientHeight;
    }

    var canvasWidth = canvas.width;
    var canvasHeight = canvas.height;

    var context = canvas.getContext('2d');
    context.clearRect(0, 0, canvasWidth, canvasHeight);

    var stickWidth = Math.min(canvasWidth / 2, canvasHeight);
    var radius = stickWidth / 2;
    var centerX = radius;
    var centerY = radius;
    var y = flightControls.throttle - 0.5;
    var x = flightControls.rudder;
    StickPresenter.render(canvas, { x: centerX, y: centerY }, radius * 0.85, { x: x, y: y }, options);
    y = flightControls.elevators;
    x = flightControls.ailerons;
    StickPresenter.render(canvas, { x: centerX * 3, y: centerY }, radius * 0.85, { x: x, y: y }, options);
};

function StickPresenter() {}

StickPresenter.render = function(canvas, center, radius, stickPosition, options) {
    var context = canvas.getContext('2d');
    context.lineWidth = 1;

    var options = options || {};
    var stickFillColor = options.stickFillColor || '#CCCCFF';
    var stickStrokeColor = options.stickStrokeColor || '#8888FF';
    var knobFillColor = options.knobFillColor || '#9494FF';
    var knobStrokeColor = options.knobStrokeColor || '#7777FF';

    context.fillStyle = stickFillColor; 
    context.strokeStyle = stickStrokeColor;

    context.beginPath();
    context.arc(center.x, center.y, radius, 0, 2 * Math.PI);
    context.fill();
    context.stroke();

    context.strokeRect(center.x - radius, center.y - radius, radius * 2, radius * 2);
    context.beginPath();
    context.moveTo(center.x, center.y - radius);
    context.lineTo(center.x, center.y + radius);
    context.moveTo(center.x - radius, center.y);
    context.lineTo(center.x + radius, center.y);
    context.stroke();

    var knobRadius = radius / 8;
    var x = center.x + 2 * radius * stickPosition.x;
    var y = center.y - 2 * radius * stickPosition.y;
    context.fillStyle = knobFillColor;
    context.strokeStyle = knobStrokeColor;

    context.beginPath();
    context.arc(x, y, knobRadius, 0, 2 * Math.PI);
    context.fill();
    context.stroke();
};