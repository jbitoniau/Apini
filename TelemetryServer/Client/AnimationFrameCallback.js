/*
    AnimationFrameCallback

    A helper class that presents the requestAnimationFrame() callback system
    in an easier form. 

    The AnimationFrameCallback is given a single user-defined callback function 
    to call regularly (typically 60Hz on most browsers/platforms).
    
    The callback starts being called only once the start() method is called.
    It can be stopped whenever with the stop() method.

    This is much easier to use than the regular requestAnimationFrame() which
    puts the burden of stopping/starting in the callback function itself (it 
    basically needs to decide whether to keep itself called or not there).
*/
function AnimationFrameCallback( userCallback )
{
    if ( !userCallback ) {
        throw new Error("Invalid argument");
    }
    this._userCallback = userCallback;
    
    this._state = 'stopped';  // 'stopped', 'running' (i.e animation frame requested and will self-maintain), 'stopping'
    this._onAnimationFrameHandler = this._onAnimationFrame.bind(this);
}

AnimationFrameCallback.prototype.start = function()
{
    if ( this._state==='stopped') 
    {
        // This is the initial animation frame request. We absolutely need to avoid requesting the animation frame
        // multiple times, otherwise the callback gets called as many time as requested for each actual event!
        this._setState('running');
        requestAnimationFrame( this._onAnimationFrameHandler );  
    }
    else if ( this._state==='running') 
    {
        // Nothing to do, already started/running
    }
    else if ( this._state==='stopping' )
    {
        // In 'stopping' mode, the callback is going to be called no matter what, and it's going to stop self-requesting.
        // Here, by switching state to 'running', we cancel the stop operation, the callback will be called and 
        // continue self-requesting. We don't have anything else to do.
        this._setState('running');
    }
    else
    {
        throw new Error("Invalid state");
    }
};

AnimationFrameCallback.prototype.stop = function()
{
    if ( this._state==='stopped') 
    {
        // Nothing to do, already stopped
    }
    else if ( this._state==='running') 
    {
        // We plan a stop in the next callback call. The callback will simply not self-request which and change state to 'stopped'
        this._setState('stopping');
    }
    else if ( this._state==='stopping' )
    {
        // Nothing to do, we've already planned a stop here in the callback
    }
    else
    {
        throw new Error("Invalid state");
    }
};

AnimationFrameCallback.prototype._setState = function( state )
{
    this._state = state;
};

// This is the method called by the system
AnimationFrameCallback.prototype._onAnimationFrame = function(timestamp)
{
    // Handle the transition from 'stopping' to 'stopped' 
    if ( this._state==='stopping' )
    {
        this._setState('stopped');
        return;
    }
    else if ( this._state!=='running' )
    {
        throw new Error("Invalid state");
    }

    // Call user-defined callback 
    this._userCallback(timestamp);

    // Continue requesting animation frame
    requestAnimationFrame( this._onAnimationFrameHandler );
};