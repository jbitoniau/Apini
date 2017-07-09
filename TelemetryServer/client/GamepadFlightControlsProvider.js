'use strict';

function GamepadFlightControlsProvider()
{
    this._flightControls = new FlightControls();
   	Object.defineProperty( this, "flightControls", { 
        writable: false,
        value: this._flightControls
    });

    this._gamepadIndex = -1;
    this._deadZoneRadius = 0.22;
}

GamepadFlightControlsProvider.prototype.update = function()
{
    if ( this._gamepadIndex===-1 )
    {
        // Check whether there's a gamepad we can use    
        this._gamepadIndex = GamepadFlightControlsProvider._getFirstAvailableGamepadIndex();
        if ( this._gamepadIndex===-1 )
        {
            // Still none available
            return;
        }
    }

    // Check whether the gamepad we're using is still there
    var gamepad = GamepadFlightControlsProvider._getGamepadByIndex( this._gamepadIndex );
    if ( !gamepad )
    {   
        // If there's another one available, use it
        this._gamepadIndex = GamepadFlightControlsProvider._getFirstAvailableGamepadIndex();
        if ( this._gamepadIndex===-1 )
        {
            // None available, reset the game controls
            this._gamepadIndex = -1;
            console.warn("Gamepad removed, no more usable gamepad. Clearing flight controls");
            this._flightControls.clear();
            return;
        }

        // Use the new one
        console.warn("Switching to a different gamepad for flight controls");
        gamepad = GamepadFlightControlsProvider._getGamepadByIndex( this._gamepadIndex );
    }

    // Movement stick
	var axes = gamepad.axes;
	var x1 = 0;
    var y1 = 0;
    if ( axes[0] )
    {
        x1 = axes[0];
    }
    if ( axes[1] )
    {
        y1 = -1 * axes[1];
    }
    var pos1 = {x:x1, y:y1} ;//GamepadFlightControlsProvider._applyDeadZoneRadius( this._deadZoneRadius, x1, y1 );
    this._flightControls.throttle = pos1.y + 0.5;
    this._flightControls.rudder = pos1.x;

    // Camera stick
   /* var xcam = 0;
    var ycam = 0;
    if ( axes[2] )
    {
        xcam = axes[2];
    }
    if ( axes[3] )
    {
        ycam = -1 * axes[3];
    }
    var cam = this._applyDeadZoneRadius( xcam, ycam );
    this._flightControls.lookUp = cam.y;
    this._flightControls.lookRight = -cam.x * 0.5;

    // Action buttons
    var buttons = gamepad.buttons;
    if ( buttons[0] )
    {
        this._flightControls.attack = buttons[0].pressed;
    }
    if ( buttons[1] )
    {
        this._flightControls.jump = buttons[1].pressed;
    }*/
};

GamepadFlightControlsProvider._applyDeadZoneRadius = function( deadZoneRadius, x, y )
{
    var radius = Math.sqrt( x*x + y*y );
    if ( radius>=deadZoneRadius )
    {
    	console.log("TODO");
    	throw new Error();
        //pos.setLength( (l - this._deadZoneRadius) / (1 - this._deadZoneRadius) );
    }
    else
    {
        x = 0;
        y = 0;
    }
    return {
    	x: x,
    	y: y
    };
};

GamepadFlightControlsProvider._getFirstAvailableGamepadIndex = function()
{
    var gamepads = GamepadFlightControlsProvider._getGamepads();
    for ( var i=0; i<gamepads.length; i++ )
    {
        if ( gamepads[i] )
        {
            return gamepads[i].index;
        }
    }    
    return -1;
};

GamepadFlightControlsProvider._getGamepadByIndex = function( index )
{
    var gamepads = GamepadFlightControlsProvider._getGamepads();
    var gamepad = gamepads[index];
    return gamepad;
};

GamepadFlightControlsProvider._getGamepads = function()
{
    var gamepads = navigator.getGamepads ? navigator.getGamepads() : (navigator.webkitGetGamepads ? navigator.webkitGetGamepads() : []);
    return gamepads;
};

