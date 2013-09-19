//instead of having function pointers in timerX_set definitions,
//we use this to avoid bug in arduino gui

typedef void (*Callback) ();

//Debugstuff
#define DEBUG_ON
#ifdef DEBUG_ON
#define DEBUG(msg,var) Serial.print(F(msg));\
Serial.println(var)
#define DEBUG(msg) Serial.println(F(msg));
#else 
#define DEBUG(msg,var) ((void)0)
#define DEBUG(msg) ((void)0)
#endif



