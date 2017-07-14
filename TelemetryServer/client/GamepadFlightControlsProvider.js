'use strict';

function GamepadFlightControlsProvider() {
    this._flightControls = new FlightControls();
    Object.defineProperty(this, 'flightControls', {
        writable: false,
        value: this._flightControls
    });

    this._gamepadIndex = -1;
    this._deadZoneRadius = 0.2;
}

GamepadFlightControlsProvider.prototype.update = function() {
    if (this._gamepadIndex === -1) {
        // Check whether there's a gamepad we can use
        this._gamepadIndex = GamepadFlightControlsProvider._getFirstAvailableGamepadIndex();
        if (this._gamepadIndex === -1) {
            // Still none available
            return;
        }
    }

    // Check whether the gamepad we're using is still there
    var gamepad = GamepadFlightControlsProvider._getGamepadByIndex(this._gamepadIndex);
    if (!gamepad) {
        // If there's another one available, use it
        this._gamepadIndex = GamepadFlightControlsProvider._getFirstAvailableGamepadIndex();
        if (this._gamepadIndex === -1) {
            // None available, reset the game controls
            this._gamepadIndex = -1;
            console.warn('Gamepad removed, no more usable gamepad. Clearing flight controls');
            this._flightControls.clear();
            return;
        }

        // Use the new one
        console.warn('Switching to a different gamepad for flight controls');
        gamepad = GamepadFlightControlsProvider._getGamepadByIndex(this._gamepadIndex);
    }

    // Get stick positions
    var axes = gamepad.axes;                // Values for axes are in the [0..1] range
    var stick1Position = { x: 0, y: 0 };
    if (axes[0]) {
        stick1Position.x = axes[0];
    }
    if (axes[1]) {
        stick1Position.y = -1 * axes[1];
    }

    var stick2Position = { x: 0, y: 0 };
    if (axes[2]) {
        stick2Position.x = axes[2];
    }
    if (axes[3]) {
        stick2Position.y = -1 * axes[3];
    }

    // Apply dead zone to both stick positions
    //console.log("GamepadFlightControlsProvider.update: stick1Position: " + JSON.stringify(stick1Position) + " stick2Position:" + JSON.stringify(stick2Position) );
    stick1Position = GamepadFlightControlsProvider._applyDeadZoneRadius(this._deadZoneRadius, stick1Position);
    stick2Position = GamepadFlightControlsProvider._applyDeadZoneRadius(this._deadZoneRadius, stick2Position);

    // Convert stick positions to flight controls
    this._flightControls.throttle = MathExtra.clamp(stick1Position.y + 0.5, 0, 1);
    this._flightControls.rudder = MathExtra.clamp(stick1Position.x, -0.5, 0.5);
    this._flightControls.elevators = MathExtra.clamp(stick2Position.y, -0.5, 0.5);
    this._flightControls.ailerons = MathExtra.clamp(stick2Position.x, -0.5, 0.5);
};

GamepadFlightControlsProvider._applyDeadZoneRadius = function(deadZoneRadius, position) {
    var result = { x: 0, y: 0 };
    var d = Math.sqrt(position.x * position.x + position.y * position.y);
    if (d >= deadZoneRadius) {
        var d2 = (d - deadZoneRadius) / (1 - deadZoneRadius);
        result.x = position.x / d * d2;
        result.y = position.y / d * d2;
    }
    return result;
};

GamepadFlightControlsProvider._getFirstAvailableGamepadIndex = function() {
    var gamepads = GamepadFlightControlsProvider._getGamepads();
    for (var i = 0; i < gamepads.length; i++) {
        if (gamepads[i]) {
            return gamepads[i].index;
        }
    }
    return -1;
};

GamepadFlightControlsProvider._getGamepadByIndex = function(index) {
    var gamepads = GamepadFlightControlsProvider._getGamepads();
    var gamepad = gamepads[index];
    return gamepad;
};

GamepadFlightControlsProvider._getGamepads = function() {
    var gamepads = navigator.getGamepads ? navigator.getGamepads() : navigator.webkitGetGamepads ? navigator.webkitGetGamepads() : [];
    return gamepads;
};
