//instead of having function pointers in timerX_set definitions,
//we use this to avoid bug in arduino gui

typedef void (*Callback) ();

//Debugstuff
#define DEBUG_ON
#ifdef DEBUG_ON
#define DEBUG_var(msg,var) Serial.print(F(msg));\
Serial.println(var)
#define DEBUG_msg(msg) Serial.println(F(msg));
#else 
#define DEBUG_var(msg,var) ((void)0)
#define DEBUG_msg(msg) ((void)0)
#endif



