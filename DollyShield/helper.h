//instead of having function pointers in timerX_set definitions,
//we use this to avoid bug in arduino gui

typedef void (*Callback) ();

//Debugstuff
#define DEBUG_ON
#ifdef DEBUG_ON
#define DEBUG(msg,var) 
Serial.print(F(msg);\
Serial.println(var)
#else DEBUG(msg,var) ((void)0)
#endif



