//instead of having function pointers in timerX_set definitions,
//we use this to avoid bug in arduino gui

typedef void (*Callback) ();

// *** DEBUG STUFF *** //
//Usage of DEBUG():
//DEBUG("message") sends message via serial
//DEBUG("message",var) sends message and value of var via serial
#define DEBUG_ON //comment/uncomment this line to enable/disable debug output!


#ifdef DEBUG_ON
#define GET_MACRO(_1,_2,NAME,...) NAME
#define DEBUG(...) GET_MACRO(__VA_ARGS__, DEBUG_var, DEBUG_msg)(__VA_ARGS__)
#define DEBUG_var(msg,var) Serial.print(F(msg));\
Serial.println(var)
#define DEBUG_msg(msg) Serial.println(F(msg));
#else 
#define DEBUG_var(msg,var) ((void)0)
#define DEBUG(...) ((void)0)
#define DEBUG_msg(msg) ((void)0)
#endif



