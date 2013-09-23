
// menu strings
const char menu_0[] PROGMEM = "Movements";
const char menu_1[] PROGMEM = "Motor  Setup";
const char menu_2[] PROGMEM = "Camera Setup";
const char menu_3[] PROGMEM = "General Setup";

const char manual_menu_0[] PROGMEM = "Manual Move";
const char manual_menu_1[] PROGMEM = "Fast Simulat.";

const char motor_menu_0[] PROGMEM = "Movement Mode";
const char motor_menu_1[] PROGMEM = "Display Mode";
const char motor_menu_2[] PROGMEM = "Ramp In Shots";
const char motor_menu_3[] PROGMEM = "Ramp Out Shots";
const char motor_menu_4[] PROGMEM = "Lead In";
const char motor_menu_5[] PROGMEM = "Lead Out";
const char motor_menu_6[] PROGMEM = "Cal. Slot";
const char motor_menu_7[] PROGMEM = "Advanced";

const char motor_adv_menu_0[] PROGMEM = "Calibrate";
const char motor_adv_menu_1[] PROGMEM = "max RPM";
const char motor_adv_menu_2[] PROGMEM = "Dist per Rev";
const char motor_adv_menu_3[] PROGMEM = "Min Cont.Speed";
const char motor_adv_menu_4[] PROGMEM = "Pulse Power";
const char motor_adv_menu_5[] PROGMEM = "Cal. Spd Low";
const char motor_adv_menu_6[] PROGMEM = "Cal. Spd Hi";

const char camera_menu_0[] PROGMEM = "Interval sec";
const char camera_menu_1[] PROGMEM = "Max Shots";
const char camera_menu_2[] PROGMEM = "Exp. Time ms";
const char camera_menu_3[] PROGMEM = "Shutter Type";
const char camera_menu_4[] PROGMEM = "Bulb Mode";
const char camera_menu_5[] PROGMEM = "Repeat";
const char camera_menu_6[] PROGMEM = "Repeat Dely ms";
const char camera_menu_7[] PROGMEM = "PreExp Dely ms";
const char camera_menu_8[] PROGMEM = "PstExp Dely ms";
const char camera_menu_9[] PROGMEM = "Focus  Dely ms";


const char set_menu_0[] PROGMEM = "Backlight";
const char set_menu_1[] PROGMEM = "AutoDim (sec)";
const char set_menu_2[] PROGMEM = "Blank LCD";
const char set_menu_3[] PROGMEM = "I/O 1";
const char set_menu_4[] PROGMEM = "I/O 2";
const char set_menu_5[] PROGMEM = "In  Delay ms";
const char set_menu_6[] PROGMEM = "Out Delay ms";
const char set_menu_7[] PROGMEM = "USB Trigger";
const char set_menu_8[] PROGMEM = "Invert Dir";
const char set_menu_9[] PROGMEM = "Invert I/O";
const char set_menu_10[] PROGMEM = "Reset Memory";

// menu organization
const char * const menu_str[] PROGMEM = { 
  menu_0,menu_1, menu_2, menu_3};

const char * const man_str[] PROGMEM = { 
  manual_menu_0,  manual_menu_1 };

const char * const motor_str[] PROGMEM = { 
  motor_menu_0,motor_menu_1, motor_menu_2, motor_menu_3, motor_menu_4, motor_menu_5, motor_menu_6,motor_menu_7};

const char * const cam_str[] PROGMEM = { 
  camera_menu_0, camera_menu_1, camera_menu_2, camera_menu_3, camera_menu_4, camera_menu_5, camera_menu_6, camera_menu_7, camera_menu_8,camera_menu_9 };

const char * const set_str[] PROGMEM = { 
  set_menu_0, set_menu_1, set_menu_2, set_menu_3, set_menu_4, set_menu_5, set_menu_6,set_menu_7, set_menu_8, set_menu_9, set_menu_10};

const char * const motor_adv_str[] PROGMEM = { 
  motor_adv_menu_0, motor_adv_menu_1,  motor_adv_menu_2,motor_adv_menu_3,motor_adv_menu_4,motor_adv_menu_5,motor_adv_menu_6};

// max number of inputs for each menu (in order listed above, starting w/ 0)
byte max_menu[7]  = {
  3,1,7,9,10,6};

//IO Mode Strings:
const char io_disabled[] PROGMEM = "Disabled";
const char io_start[] PROGMEM = "Start";
const char io_stop[] PROGMEM = "Stop";
const char io_toggle[] PROGMEM = "Toggle";
const char io_toggledir[] PROGMEM = "Change Dir";
const char io_extintv[] PROGMEM = "Ext. Interval.";
const char io_outbefore[] PROGMEM = "Out Before";
const char io_outafter[] PROGMEM = "Out After";
const char io_outboth[] PROGMEM = "Out Both";
const char * const io_strings[] PROGMEM = {io_disabled,io_start,io_stop,io_toggle,io_toggledir,io_extintv,io_outbefore,io_outafter,io_outboth};
  
//Shutter Mode Strings
const char shutter_cablefocus[] PROGMEM = "Cable+Focus";
const char shutter_cable[] PROGMEM = "Cable";
const char shutter_ircanon[] PROGMEM = "IR Canon";
const char shutter_irnikon[] PROGMEM = "IR Nikon";
const char * const shutter_strings[] PROGMEM = {shutter_cablefocus,shutter_cable,shutter_ircanon,shutter_irnikon};
