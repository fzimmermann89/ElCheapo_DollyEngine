/* 
 
 MX2 El Cheapo
 modified Version of Dynamic Perception LLC's DollyShield
 (c) 2010-2011 C.A. Church / Dynamic Perception LLC
 (c) FFZ
 For more info go to http://openmoco.org or http://www.thundercorp.de/timelapse
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

/*
NOTES:
 -use timer2 for lcd and motor "soft pwm" to use any pin. in slow pulse mode use counter in isr. less overhead than timer1
 -user timer1 in ctc mode with couter for general purpose ms-timer, instead of mstimer2 OR as compare..
 -check bulb mode/stop camera using ir
 
 timer1 thoughts:
 16bit timer, dh bis 65535.
 bei 16mhz:
 prescaler		zeit pro tick		max zeit		ticks pro ms
 1				0,0625us			4ms				16000
 8				0,5us				32ms			2000
 64				4us					262ms			250
 256			16us				1s				62.5	~63 	wert=wert<<6-wert
 1024			64us				4s				15.625  ~16		wert=wert<<4
 
 
 */

#define EEPROM_TODO 123




#include <avr/pgmspace.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <digitalWriteFast.h>
#include "MsTimer2.h"
#include "TimerOne.h"



#define FIRMWARE_VERSION  92

// motor PWM
#define MOTOR0_P 3
#define MOTOR0_DIR 13

// camera pins
#define CAMERA_PIN 12
#define FOCUS_PIN 15
#define IR_PIN 12

#define FREQ 38400
#define oscd 16 //TODO

#define MAX_MOTORS 1

//#define MP_PERIOD 30
#define MP_PERIOD 30


/* User Interface Values */

// lcd pins
#define LCD_RS  8
#define LCD_EN  9
#define LCD_D4  4
#define LCD_D5  5
#define LCD_D6  6
#define LCD_D7  7

// which input is our button
#define BUT_PIN A0

// lcd backlight pin
#define LCD_BKL 10

// max # of LCD characters (including newline)
#define MAX_LCD_STR 17



// how many buttons dow we have?
#define NUM_BUTTONS 5

// button return values

#define BUT0  1
#define BUT1  2
#define BUT2  3
#define BUT3  4
#define BUT4  5

// which buttons?
#define BUT_UP BUT1
#define BUT_DN BUT2
#define BUT_CT BUT4
#define BUT_LT BUT3
#define BUT_RT BUT0

// analog button read values
#define BUT0_VAL  0
#define BUT1_VAL  100
#define BUT2_VAL  255
#define BUT3_VAL  405
#define BUT4_VAL  640

// button variance range
#define BUT_THRESH  40


// how many ms does a button have
// to be held before triggering another
// action? (for scrolling, etc.)

#define HOLD_BUT_MS 200

// how much to increment for each cycle the button is held?

#define HOLD_BUT_VALINC 10

// ALT input debouncing time

#define ALT_TRIG_THRESH 250

//IR Sequence
unsigned int seq[] = {
  16,77,1069,15,61,16,137,15,2427,77,1069,15,61,16,10};

// menu strings
prog_char menu_0[] PROGMEM = "Movements";
prog_char menu_1[] PROGMEM = "Motor  Setup";
prog_char menu_2[] PROGMEM = "Camera Setup";
prog_char menu_3[] PROGMEM = "General Setup";

prog_char manual_menu_0[] PROGMEM = "Manual Move";
prog_char manual_menu_1[] PROGMEM = "Fast Simulat.";

prog_char axis_menu_0[] PROGMEM = "Ramp In Shots";
prog_char axis_menu_1[] PROGMEM = "Ramp Out Shots";
prog_char axis_menu_2[] PROGMEM = "Angel";
prog_char axis_menu_3[] PROGMEM = "Lead In";
prog_char axis_menu_4[] PROGMEM = "Lead Out";
prog_char axis_menu_5[] PROGMEM = "Advanced";

prog_char axis_adv_menu_0[] PROGMEM = "Calibrate";
prog_char axis_adv_menu_1[] PROGMEM = "max RPM";
prog_char axis_adv_menu_2[] PROGMEM = "Dist per Rev";
prog_char axis_adv_menu_3[] PROGMEM = "Min Cont. Speed";
prog_char axis_adv_menu_4[] PROGMEM = "Pulse Length";
prog_char axis_adv_menu_5[] PROGMEM = "Pulse Power";
prog_char axis_adv_menu_6[] PROGMEM = "Cal. Spd Low";
prog_char axis_adv_menu_7[] PROGMEM = "Cal. Spd Hi";

prog_char camera_menu_0[] PROGMEM = "Interval sec";
prog_char camera_menu_1[] PROGMEM = "Max Shots";
prog_char camera_menu_2[] PROGMEM = "Exp. Time ms";
prog_char camera_menu_3[] PROGMEM = "Shutter Type";
prog_char camera_menu_4[] PROGMEM = "Bulb Mode";
prog_char camera_menu_5[] PROGMEM = "Repeat";
prog_char camera_menu_6[] PROGMEM = "Repeat Delay ms";
prog_char camera_menu_7[] PROGMEM = "PreExp Delay ms";
prog_char camera_menu_8[] PROGMEM = "PstExp Delay ms";
prog_char camera_menu_9[] PROGMEM = "Focus Delay ms";


prog_char set_menu_0[] PROGMEM = "Backlight";
prog_char set_menu_1[] PROGMEM = "AutoDim (sec)";
prog_char set_menu_2[] PROGMEM = "Blank LCD";
prog_char set_menu_3[] PROGMEM = "I/O 1";
prog_char set_menu_4[] PROGMEM = "I/O 2";
prog_char set_menu_5[] PROGMEM = "In Delay ms";
prog_char set_menu_6[] PROGMEM = "Out Delay ms";
prog_char set_menu_7[] PROGMEM = "USB Trigger";
prog_char set_menu_8[] PROGMEM = "Invert Dir";
prog_char set_menu_9[] PROGMEM = "Invert I/O";
prog_char set_menu_10[] PROGMEM = "Reset Mem";

// menu organization

PROGMEM const char *menu_str[]  = { 
  menu_0,menu_1, menu_2, menu_3};

PROGMEM const char *man_str[]   = { 
  manual_menu_0,  manual_menu_1 };

PROGMEM const char *axis0_str[] = { 
  axis_menu_0,axis_menu_1, axis_menu_2, axis_menu_3, axis_menu_4, axis_menu_5};

PROGMEM const char *cam_str[]   = { 
  camera_menu_0, camera_menu_1, camera_menu_2, camera_menu_3, camera_menu_4, camera_menu_5, camera_menu_6, camera_menu_7, camera_menu_8,camera_menu_9 };

PROGMEM const char *set_str[]   = { 
  set_menu_0, set_menu_1, set_menu_2, set_menu_3, set_menu_4, set_menu_5, set_menu_6,set_menu_7, set_menu_8, set_menu_9, set_menu_10};

PROGMEM const char *axis_adv_str[]   = { 
  axis_adv_menu_0, axis_adv_menu_1,  axis_adv_menu_2,axis_adv_menu_3,axis_adv_menu_4,axis_adv_menu_5,axis_adv_menu_6,axis_adv_menu_7};

// max number of inputs for each menu (in order listed above, starting w/ 0)
byte max_menu[7]  = {
  3,1,5,9,10,7};

// support a history of menus visited up to 5 levels deep
byte hist_menu[5] = {
  0,0,0,0,0};


//Special Return Codes used / Magic Values
#define MENU_MANUAL 254
#define MENU_CALIBRATION 253
#define MENU_INPUT 255

char lcd_buf[MAX_LCD_STR];

// what is our currently selected menu?
// what is our current position?
byte cur_menu      = 0;
byte cur_pos       = 0;
byte cur_pos_sel   = 0;

// which input value position are we in?
byte cur_inp_pos   = 0;

// input buffers
unsigned long cur_inp_long  = 0;
float cur_inp_float         = 0.0;
boolean cur_inp_bool        = false;

// which input are we on, if on
// the main screen.
byte main_scr_input         = 0;

// last read button (analog) value
int last_but_rd = 1013;

// flags for each button
// use indivial bits to indicate
// whether a given button was pressed.
byte button_pressed = 0;

// input value multiplier
unsigned int inp_val_mult = 1;

// how long has a button been held for?
unsigned long hold_but_tm = 0;

// when was ui last updated on home scr?
unsigned long ui_update_tm = 0;

// lcd dim control
byte cur_bkl     = 255;
boolean blank_lcd   = false;

// for dimming lcd
unsigned int lcd_dim_tm     = 5;
unsigned long input_last_tm = 0;

// show cm instead of inch?
//boolean ui_is_metric = false;

// invert L/R displays?
boolean ui_invdir = false;

// floats are input in tenths?
boolean ui_float_tenths = false;

// user interface control flags
 #define UI_UPDATE_DISP (1<<0)        //B0 = update display
 #define UI_SETUP_MENU (1<<1)         //B1 = currently in setup menu
 #define UI_VALUE_ENTRY (1<<2)        //B2 = in value entry
 #define UI_DRAWN_INITAL_VALUE (1<<3) //B3 = have drawn initial value in value entry
 #define UI_USED_DECIMAL (1<<4)       //B4 = have used decimal in current value
 #define UI_MANUAL_MODE (1<<5)        //B5 = in manual mode
 #define UI_LCD_BKL_ON (1<<6)         //B6 = lcd bkl on
 #define UI_CALIBRATE_MODE (1<<7)     //B7 = in calibrate mode
byte ui_ctrl_flags = B00000000;

/* calibration screen flags
 
 B0 = Currently calibrating
 B1 = Done Calibrating
 
 */
byte ui_cal_scrn_flags = 0;

// whether to show cpm (true) or % (false)
boolean ui_motor_display = true;
#define CPM true
#define PCT false

//input type flags

 enum  __attribute__((packed)) INPUTS {
     INPUT_FLOAT, INPUT_LONG, INPUT_ONOF, INPUT_SHUTTER, INPUT_LTRT, INPUT_CMPCT,INPUT_CONTSMS,INPUT_ANGEL,INPUT_IO,INPUT_SPEED,INPUT_PREPOST
 };


byte ui_type = 0;

// run status flags
typedef struct
{
unsigned char bit0:
  1;
unsigned char bit1:
  1;
unsigned char bit2:
  1;
unsigned char bit3:
  1;
unsigned char bit4:
  1;
unsigned char bit5:
  1;
unsigned char bit6:
  1;
unsigned char bit7:
  1;
}
io_reg;

#define S_RUNNING  		 ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit0
#define S_CAM_ENGAGED   	 ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit1
#define S_CAM_CYCLE_COMPLETE   	 ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit2
#define S_MOT_RUNNING   	 ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit3
#define S_EXT_TRIG_ENGAGED   	 ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit4
#define BIT_6   		 ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit5
#define BIT_7  			 ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit6
#define BIT_8			 ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit7

/* 
 B0 = running
 B1 = camera currently engaged
 B2 = camera cycle complete
 B3 = motor currently running
 B4 = external trigger engaged
 b5 = slow mode
 b6 = motor ran
 b7 
 */

// external intervalometer
#define EXT_INTV_1 (1 << 0) //B0 = I/O 1 is external intervalometer
#define EXT_INTV_2 (1 << 1) //B1 = I/O 2 is external intervalometer
#define EXT_INT_OK (1 << 2) //B2 = interval OK to fire
byte external_interval = 0;

//external trigger via alt i/o pins
#define EXT_TRIG_1_BEFORE (1 << 0) //B0 = I/O 1 external enabled (before)
#define EXT_TRIG_2_BEFORE (1 << 1) //B1 = I/O 2 external enabled (before)
#define EXT_TRIG_1_AFTER  (1 << 2) //B2 = I/O 1 external enabled (after)
#define EXT_TRIG_2_AFTER  (1 << 2) //B3 = I/O 2 external enabled (after)
byte external_trigger  = 0;

// trigger delays
unsigned long ext_trig_pre_delay = 0;
unsigned long ext_trig_pst_delay = 0;

// camera exposure time
unsigned long exp_tm      = 100;

// tap focus before exposing
unsigned int focus_tap_tm = 0;

// delay after exposing (mS)
unsigned int post_delay_tm      = 100;


 enum  __attribute__((packed)) SHUTTER_MODE {
     SHUTTER_MODE_IR_NIKON=0, SHUTTER_MODE_IR_CANON=1, SHUTTER_MODE_CABLE_NO_FOCUS=2, SHUTTER_MODE_CABLE_FOCUS=3};

// shutter mode (0:NikonIR/1:CanonIR/2:Shutter Cab/3:Shut+Foc Cab)
byte shutter_mode   = 0;

// intervalometer time (seconds)
float cam_interval = 1.0;

// max shots
unsigned int cam_max  = 0;

// camera repeat shots
byte cam_repeat = 0;

// delay between camera repeat cycles
unsigned int cam_rpt_dly = 250;

byte pre_focus_clear      = 0;
unsigned long cam_last_tm = 0;

// currently selected motor
//byte cur_motor = 0;

// set speed for the current motor
unsigned int m_speed = 0;

// currently set speed (for altering motor speed)
unsigned int mcur_spds=0;

// prev direction for motor
byte m_wasdir = 0;

// distance (i) per revolution
float m_diarev =  3.53;

// motor RPMs
float m_rpm    = 9;

// calculated max cpm
float max_cpm =  m_diarev * m_rpm;

// user-configurable min cpm
float min_cpm = 20.0;

// minimumspeed (min cpm->255 scale value) //TODO WHY?
byte min_spd =  (min_cpm / max_cpm) * 255;

// minimum pulse cycles per motor
byte m_min_pulse=   20;

// calibration points
byte motor_spd_cal[2] = {
  2,10};

// maximum sms distance //TODO
unsigned int m_maxsms =  max_cpm * 100;

/*
// for timer1 pulsing mode control
 boolean timer_used = false; */
 //volatile  bool timer_engaged      = false;
 volatile bool motor_engaged      = false;
 volatile bool motor_ran = 0;

//TODO

// motor calibration
float m_cal_constant = 0.69;
float m_cal_array[3][3][2] = { //TODO sinnvolle Werte voreintragen. 

  {
    {
      0.61914329,0.61914329                      }
    ,{
      1.0,1.0                      }
    ,{
      2.01133251,2.11453032                      }
  }
  ,
  {
    {
      0.61914329,0.61914329                      }
    ,{
      1.0,1.0                      }
    ,{
      2.01133251,2.11453032                      }
  }
  ,
  {
    {
      0.61914329,0.61914329                      }
    ,{
      1.0,1.0                      }
    ,{
      2.01133251,2.11453032                      }
  } 
}
;
//aktueller Winkel-wert in m_cal_array für den kalibriert wird //warum global? //TODO
byte m_cur_cal = 0;     

byte m_angle = 0;

boolean m_cal_done = false;

// ramping data
byte m_ramp_set     = 0;
float m_ramp_shift  = 0.0;
byte m_ramp_mod	    = 0;

// lead-ins for axis movement
unsigned int m_lead_in  = 0;
unsigned int m_lead_out = 0;

// for controlling pulsing and sms movement
unsigned long on_pct  = 0;
unsigned long off_pct = 0;
unsigned int m_sms_tm = 0;

// shots fired
unsigned long shots = 0;

// function types for alt inputs...
/* 
 0 = disabled
 1 = start
 2 = stop
 */
byte input_type[2]            = {
  0,0};
unsigned long input_trig_last = 0;

// usb trigger flag
boolean gb_enabled = false;

// default alt I/O rising/falling direction
byte altio_dir = FALLING;

// initialize LCD object
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() { 

  pinModeFast(CAMERA_PIN, OUTPUT);
  pinModeFast(FOCUS_PIN, OUTPUT);
  pinModeFast(IR_PIN, OUTPUT);
  pinModeFast(MOTOR0_P, OUTPUT);
  pinModeFast(MOTOR0_DIR, OUTPUT);

  Serial.begin(115200);
  init_user_interface();

  // check firmware version stored in eeprom
  // will cause eeprom_saved() to return false
  // if version stored in eeprom does not match
  // firmware version.  This automatically clears
  // saved memory after a new firmware load -
  // saving lots of support questions =)

  eeprom_versioning();

  // did we previously save settings to eeprom?
  if( eeprom_saved() ) {
    // restore saved memory
    restore_eeprom_memory();
  }
  else {
    // when memory has been cleared, or nothing has been
    // saved, make sure eeprom contains default values
    write_all_eeprom_memory();
  }

  input_last_tm=millis();

  show_home();

  //Output Calibration Data
  for( byte i = 0; i <= 2; i++) {
    Serial.print(i, DEC);
    Serial.print(":");
    for ( byte x = 0; x < 2; x++ ) {
      Serial.print(m_cal_array[0][i][x], 8);
      Serial.print(":");
    }
    Serial.println("");
  } 

}

void loop() {

  // check for signal from gbtimelapse serial command.
  // we check here to prevent queuing commands when stopped

  if( gb_enabled == true && gbtl_trigger() == true ) {
    external_interval |= B00100000;
  }

  if( S_RUNNING ) { //run_status & B10000000
    // program is running
    main_loop_handler();
  } // end if running


  // always check the UI for input or
  // updates

  check_user_interface();    

}

void main_loop_handler() {


  static boolean camera_fired   = false;
  static boolean motors_clear   = false;
  static boolean ok_stop        = false;
  static boolean in_sms_cycle   = false;
  static boolean do_fire        = false;
  static boolean ext_trip       = false;
  static byte    cam_repeated   = 0;


  if( cam_max > 0 && shots >= cam_max && ( ok_stop || (m_speed <= 0.0 ) || motor_sl_mod ) ) {
    // stop program if max shots exceeded, and complete cycle completed
    // if in interleave, ignore complete cycle if in pulse
    ok_stop = false;
    stop_executing();
    // interrupt further processing      
  }

  else  if ( (m_speed > 0) && (m_speed < min_spd ) )  {
    // if pulse mode is on and
    //motor needs to be pulsed...
    motor_run_pulsing();

  }



  // we need to determine if we can shoot the camera
  // by seeing if it is currently being fired, or 
  // is blocked in some way.  After making sure we're
  // not blocked, we check to see if its time to fire the
  // camera

  if( motor_engaged && motor_ran ) {      
      // motor has run one
      // cycle, let the camera fire
      motor_engaged = false;
      ok_stop       = true;
      in_sms_cycle  = false;
    }
  } // end if motor_engaged && motor_ran
  
  
  else if( S_CAM_ENGAGED) { //run_status & B01001000
    // currently firing the camera
    // do nothing
    ;
  }
  else if(S_CAM_CYCLE_COMPLETE) { //run_status & B00100000
    // camera cycle completed
    // clear exposure cycle complete flag
   S_CAM_CYCLE_COMPLETE=false;
   
    if( camera_fired == true ) {
      // the shot just fired
      camera_fired = false;
      shots++;


      // for ramping motor speeds
      // we change speed in ramps after shots...
      motor_execute_ramp_changes();
      
      
      // check to see if a post-exposure delay is needed
      if( post_delay_tm > 0 ) {
        // we block anything from happening while in the
        // post-exposure cycle by pretending to be an
        // exposure
      S_CAM_ENGAGED=true;  

	   //TODO
       // MsTimer2::set(post_delay_tm, camera_clear);
       // MsTimer2::start();

        motors_clear = false;
        ok_stop = false;
      }
      else {
        // no post-exp delay, is the external trigger to fire?
        if( external_trigger & (EXT_TRIG_2_AFTER | EXT_TRIG_1_AFTER) )  // && ext_trig_pst_delay > 0 
          alt_ext_trigger_engage(false);


        //no post-exposure delay, motors can run
        motors_clear = true;
      }
    }//end cam_fired==true 
    else {
      // this was a post-exposure delay cycle completing, not
      // an actual shot
  /* //TODO //why? already done?
      // is the external trigger to fire?
      if( external_trigger & B00110000 && ext_trig_pst_delay > 0 )
        alt_ext_trigger_engage(false);
*/

      // we can set the motors clear to move now
      motors_clear = true;        
    }
  }//end S_CAM_CYCLE_COMPLETE

  //else if( motors_clear == true && !motor_sl_mod && ( m_sms_tm > 0 ) ) {

    //// if we're set to go to s-m-s and motor is set to move
    //// start motor moving

    //motor_ran = 0;

    //// set motor to move, and then
    //// set timer to turn them off  

    //if( m_sms_tm > 0 ) {
      //// start motor
      //run_motor_sms(); 
      //MsTimer2::set(m_sms_tm, stop_motor_sms);
    //}


    //// engage timer
    //MsTimer2::start();

    //motor_engaged = true;
    //motors_clear = false;
    //ok_stop      = false;

  //}   
  
  else if( gb_enabled == true || external_interval & (EXT_INTV_1|EXT_INTV_2) ) {
    // external intervalometer is engaged

    if( external_interval & EXT_INT_OK ) {
      // external intervalometer has triggered

      // clear out ok to fire flag
      external_interval &= ~ẼXT_INTV_OK;      
      do_fire = true;
    }
  }
  
  else if( cam_last_tm < millis() - (cam_interval * 1000) ) {
    // internal intervalometer triggers
    do_fire = true;
  }

  if( do_fire == true ) {
    // we've had a fire camera event

    // is the external trigger to fire? (either as 'before' or 'through')
    if( (external_trigger & (EXT_TRIG_1_BEFORE|EXT_TRIG_2_BEFORE))  && ext_trip == false && (cam_repeat == 0 || cam_repeated == 0) ) {
      alt_ext_trigger_engage(true);
      ext_trip = true;
    }
    else {

      // make sure we handle pre-focus tap timing

      if( ( pre_focus_clear == 4 || focus_tap_tm == 0 || (cam_repeat > 0 && cam_repeated > 0) ) && !(S_EXT_TRIG_ENGAGED) ) { //run_status & B00001000

        // we always set the start mark at the time of the
        // first exposure in a repeat cycle (or the time of exp
        // if no repeat cycle is in play

        if( cam_repeat == 0 || cam_repeated == 0 )
          cam_last_tm  = millis();

        // deal with camera repeat actions
        if( cam_repeat == 0 || (cam_repeat > 0  && cam_repeated >= cam_repeat) ) {
          camera_fired = true;
          do_fire = false;
          ext_trip = false;
          cam_repeated = 0;
        }
        else if( cam_repeat > 0 ) {
          // only delay ater the first shot
          if( cam_repeated > 0 )
            delay(cam_rpt_dly); // blocking delay between camera firings (we should fix this later!)

          cam_repeated++;
        }

        // camera is all clear to fire, and enough
        // time is elapsed
        fire_camera(exp_tm);
        pre_focus_clear = 0;

      }
      else if( focus_tap_tm > 0 && pre_focus_clear == 0 && !(S_EXT_TRIG_ENGAGED) ) { //run_status & B00001000
        // pre-focus tap is set, bring focus line high
        digitalWriteFast(FOCUS_PIN, HIGH);
        MsTimer2::set(focus_tap_tm, stop_cam_focus);
        MsTimer2::start();
        pre_focus_clear = 1;
      }
    } // end else (not external trigger...
  } // end if(do_fire...
}

void start_executing() {
  // starts program execution

  // clear out external interval flag in case it was
  // set while stopped.

  external_interval &= B11011111;

 
S_RUNNING=true; //run_status |= B10010000;
S_MOT_RUNNING=true; //run_status |= B10010000;
 
 // turn on motors
  motor_control(true);


  // if ramping is enabled for a motor, start at a zero
  // speed
  if( m_ramp_set[0] >= 1 )
    motor_set_speed(0); 


  // reset shot counter
  shots = 0;
}

void stop_executing() {
  //run_status &= B01100111;
  S_RUNNING=false;
  S_MOT_RUNNING=false;
  S_EXT_TRIG_ENGAGED=false;
  motor_stop_all();
}

boolean gbtl_trigger() {

  if( Serial.available() > 0 ) {
    char thsChar = Serial.read();

    if( thsChar == 'T' ) {
      return true;
    }
    else {
      return false;
    }
  }

  return false;
}

