'use strict';

if ( typeof require==="function" ) {
    var MathExtra = require('./MathExtra').MathExtra;
}

function FlightControls() {
    var throttle = 0;
    Object.defineProperty(this, 'throttle', {
        enumerable: true,
        get: function() {
            return throttle;
        },
        set: function(value) {
            if (typeof value !== 'number') throw new Error('Invalid argument');
            if ( value<0 || value>1){
                console.warn('FlightControls.throttle: value is out of range');
            }
            value = MathExtra.clamp(value, 0, 1);
            throttle = value;
        }
    });

    var rudder = 0;
    Object.defineProperty(this, 'rudder', {
        enumerable: true,
        get: function() {
            return rudder;
        },
        set: function(value) {
            if (typeof value !== 'number') throw new Error('Invalid argument');
            if ( value<-0.5 || value>0.5){
                console.warn('FlightControls.rudder: value is out of range');
            }
            value = MathExtra.clamp(value, -0.5, 0.5);
            rudder = value;
        }
    });

    var elevators = 0;
    Object.defineProperty(this, 'elevators', {
        enumerable: true,
        get: function() {
            return elevators;
        },
        set: function(value) {
            if (typeof value !== 'number') throw new Error('Invalid argument');
            if ( value<-0.5 || value>0.5){
                console.warn('FlightControls.elevators: value is out of range');
            }
            value = MathExtra.clamp(value, -0.5, 0.5);
            elevators = value;
        }
    });

    var ailerons = 0;
    Object.defineProperty(this, 'ailerons', {
        enumerable: true,
        get: function() {
            return ailerons;
        },
        set: function(value) {
            if (typeof value !== 'number') throw new Error('Invalid argument');
            if ( value<-0.5 || value>0.5){
                console.warn('FlightControls.ailerons: value is out of range');
            }
            value = MathExtra.clamp(value, -0.5, 0.5);
            ailerons = value;
        }
    });

    var pTerm = 0;
    Object.defineProperty(this, 'pTerm', {
        enumerable: true,
        get: function() {
            return pTerm;
        },
        set: function(value) {
            if (typeof value !== 'number') throw new Error('Invalid argument');
            if ( value<0 || value>1){
                console.warn('FlightControls.pTerm: value is out of range');
            }
            value = MathExtra.clamp(value, 0, 1);
            pTerm = value;
        }
    });

    var iTerm = 0;
    Object.defineProperty(this, 'iTerm', {
        enumerable: true,
        get: function() {
            return iTerm;
        },
        set: function(value) {
            if (typeof value !== 'number') throw new Error('Invalid argument');
            if ( value<0 || value>1){
                console.warn('FlightControls.iTerm: value is out of range');
            }
            value = MathExtra.clamp(value, 0, 1);
            iTerm = value;
        }
    });

    var dTerm = 0;
    Object.defineProperty(this, 'dTerm', {
        enumerable: true,
        get: function() {
            return dTerm;
        },
        set: function(value) {
            if (typeof value !== 'number') throw new Error('Invalid argument');
            if ( value<0 || value>1){
                console.warn('FlightControls.dTerm: value is out of range');
            }
            value = MathExtra.clamp(value, 0, 1);
            dTerm = value;
        }
    });
}

FlightControls.prototype.clear = function() {
    this.throttle = 0;
    this.rudder = 0;
    this.elevators = 0;
    this.ailerons = 0;
    this.pTerm = 0;
    this.iTerm = 0;
    this.dTerm = 0;
};

// Copy the state of a given FlightControls object into this one
FlightControls.prototype.copy = function(other) {
    this.throttle = other.throttle;
    this.rudder = other.rudder;
    this.elevators = other.elevators;
    this.ailerons = other.ailerons;
    this.pTerm = other.pTerm;
    this.iTerm = other.iTerm;
    this.dTerm = other.dTerm;
};

// Combines the state of a given FlightControls object into this one
// This method is used when multiple sources of FlightControls exist
// and need to be combined intelligently into a single one.
FlightControls.prototype.combine = function(other) {
    this.throttle = MathExtra.clamp(this.throttle + other.throttle, 0, 1);
    this.rudder = MathExtra.clamp(this.rudder + other.rudder, -0.5, 0.5);
    this.elevators = MathExtra.clamp(this.elevators + other.elevators, -0.5, 0.5);
    this.ailerons = MathExtra.clamp(this.ailerons + other.ailerons, -0.5, 0.5);

    // Don't touch pTerm, iTerm, etc...!
};

if ( typeof module!=="undefined" && ('exports' in module) ){
    exports.FlightControls = FlightControls;
}

