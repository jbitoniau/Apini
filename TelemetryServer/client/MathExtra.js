'use strict';

function MathExtra()
{
}

MathExtra.clamp = function( value, min, max) {
	if ( typeof value!=='number' || typeof min!=='number' || typeof max!=='number' || max<min )
		throw new Error("Invalid argument");
	
	if ( value<min ) {
		value = min;
	}
	else if (value>max) {
		value = max;
	}
	return value;
};

// roundValueTo( 13.123 ) returns 13
// roundValueTo( 13, 5 ) returns 15
// roundValueTo( -2.52, 5 ) return -5
// roundValueTo( -2.49, 5 ) return 0
MathExtra.roundValueTo = function( value, step )
{
    var v = 0;
    if ( !step )
    {
        v = Math.round( value );
    }
    else
    {
        v = Math.round( value / step ) * step;
    }
    return v;
};