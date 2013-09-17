//instead of having function pointers in timerX_set definitions,
//we use this to avoid bug in arduino gui

typedef void (*Callback) ();



