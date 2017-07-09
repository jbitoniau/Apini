'use strict';

function FlightControls()
{
    var throttle = 0;
    Object.defineProperty( this, "throttle", { 
        get: function()
        {
            return throttle;
        },
        set: function( value ) {
            if ( typeof value!=='number' )
                throw new Error("Invalid argument");
            value = MathExtra.clamp( value, 0, 1 );
            throttle = value;
        }
    });

    var rudder = 0;
    Object.defineProperty( this, "rudder", { 
        get: function()
        {
            return rudder;
        },
        set: function( value ) {
            if ( typeof value!=='number' )
                throw new Error("Invalid argument");
            value = MathExtra.clamp( value, -0.5, 0.5 );
            rudder = value;
        }
    });
}

FlightControls.prototype.clear = function()
{
    this.throttle = 0;
    this.rudder = 0;
};

// Copy the state of a given FlightControls object into this one
FlightControls.prototype.copy = function( other )
{
    this.throttle = other.throttle;
    this.rudder = other.rudder;
};

// Combines the state of a given FlightControls object into this one
// This method is used when multiple sources of FlightControls exist
// and need to be combined intelligently into a single one.
FlightControls.prototype.combine = function( other )
{
    this.throttle = MathExtra.clamp( this.throttle + other.throttle, 0, 1 );
    this.rudder = MathExtra.clamp( this.rudder + other.rudder, -0.5, 0.5 );
};
