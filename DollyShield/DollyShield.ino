/* 
 
 El Cheapo Dollyshield
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
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <LiquidCrystal.h>
#include "digitalWriteFastMod.h"

#include "helper.h"
#include <limits.h>
#include "pinout.h"
#include "strings.h"

const uint8_t FIRMWARE_VERSION = 01;

// max # of LCD characters (including newline)
const uint8_t MAX_LCD_STR = 17;

//IR Settings
const uint8_t FREQ = 38400;
const uint8_t OSCD = 16; //TODO

//IR sequences
//NIKON
unsigned int const seq_nikon[]={14,77,1069,15,61,16,137,15,2427,77,1069,15,61,16,10};
//CANON
unsigned int const seq_canon[]={3,16,458,16};
unsigned int const *seqs[] = {seq_nikon,seq_canon};
const uint8_t IR_NIKON = 0;
const uint8_t IR_CANON = 1;

// support a history of menus visited up to 5 levels deep
byte hist_menu[5] = {
  0,0,0,0,0};

// lcd buffer
char lcd_buf[MAX_LCD_STR];


// what is our currently selected menu?
// what is our current position?
byte cur_menu      = MENU_MAIN;
byte cur_pos       = 0;
byte cur_first_item   = 0;

// which input value position are we in?
byte cur_inp_pos   = 0;

// input buffers
unsigned int cur_inp_int  = 0;
unsigned int cur_inp_int_max = UINT_MAX;
unsigned int cur_inp_int_min = 0;

float cur_inp_float         = 0.0;
float cur_inp_float_max     = UINT_MAX;
float cur_inp_float_min     = 0.0;

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
byte lcd_bkl     = 255;
boolean blank_lcd   = false;

// for dimming lcd
byte lcd_dim_tm     = 0;
unsigned long input_last_tm = 0;

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

//calibration screen flags
#define CAL_CALIBRATING (1<<0)     //B0 = Currently calibrating
#define CAL_DONE        (1<<1)     //B1 = Done Calibrating
#define CAL_STEP_DONE   (1<<2)     //B2 = Current Calibration Step finished
byte cal_flags = 0;

// whether to show cpm (true) or % (false)
boolean ui_motor_display = true;
#define CPM true
#define PCT false

//input type flags

 enum  __attribute__((packed)) INPUTS {
     INPUT_FLOAT, INPUT_UINT, INPUT_ONOFF, INPUT_SHUTTER, INPUT_LTRT, INPUT_CMPCT,INPUT_CONTSMS, INPUT_SLOT,INPUT_IO, INPUT_OKCANCEL
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

#define S_IN_DELAY                    ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit0
#define S_SLOW_MODE_MON               ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit1
#define S_SLOW_MODE                   ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit2
#define S_MOT_RUNNING                 ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit3
#define S_DELAYS_DONE                 ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit4
#define S_TIMER1_SET                  ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit5
#define S_TIMER2_SET                  ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit6
#define S_TIMER3_SET                  ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR0))->bit7

#define S_RUNNING                     ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR1))->bit0
#define S_CAM_ENGAGED                 ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR1))->bit1
#define S_CAM_CYCLE_COMPLETE          ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR1))->bit2
#define S_EXT_TRIG_SETUP              ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR1))->bit3
#define S_4                           ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR1))->bit4
#define S_3                           ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR1))->bit5
#define S_2                           ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR1))->bit6
#define S_1                           ((volatile io_reg*)_SFR_MEM_ADDR(GPIOR1))->bit7

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
#define EXT_INTV_1   (1 << 0) //B0 = I/O 1 is external intervalometer
#define EXT_INTV_2   (1 << 1) //B1 = I/O 2 is external intervalometer
#define EXT_INTV_USB (1 << 2) //B1 = USB Trigger enabled
#define EXT_INTV_OK  (1 << 3) //B2 = interval OK to fire
//external trigger via alt i/o pins
#define EXT_TRIG_1_BEFORE (1 << 4) //B0 = I/O 1 external enabled (before)
#define EXT_TRIG_2_BEFORE (1 << 5) //B1 = I/O 2 external enabled (before)
#define EXT_TRIG_1_AFTER  (1 << 6) //B2 = I/O 1 external enabled (after)
#define EXT_TRIG_2_AFTER  (1 << 7) //B3 = I/O 2 external enabled (after)
byte external_io = 0;

// camera exposure time
unsigned long exp_tm      = 100;


//delays in ms
unsigned int delay_postexp   = 100;    //Post Exposue
unsigned int delay_preexp    = 0;      //Pre Expose
unsigned int delay_ext_in    = 0;      //Delay after Intervalometer till shot
unsigned int delay_ext_out   = 200;      //Time ext_trigger is enabled before or after shot
unsigned int length_ext_out  = 100;    //Time for which the trigger is enabled
unsigned int delay_focus     = 0;      //Time to wait after focus signal has been sent
unsigned int delay_repeat    = 250;    // delay between camera repeat cycles
/*
// TODO delay status flags
#define DELAY_IN     (1 << 0) //B0 = currently in any delay
#define DELAY_DONE_FOCUS
#define DELAY_DONE_PREEXP  (1 << 1) //B1 = In pre exposure delay
#define DELAY_DONE_POSTEXP (1 << 2) //B2 = In post exposure delay
#define DELAY_DONE_REPEAT  (1 << 2) //B2 = In delay between repeats
byte delay_status = 0;
*/

// shutter mode
 enum  __attribute__((packed)) SHUTTER_MODE {
     SHUTTER_MODE_IR_NIKON=0, SHUTTER_MODE_IR_CANON=1, SHUTTER_MODE_CABLE_NO_FOCUS=2, SHUTTER_MODE_CABLE_FOCUS=3};
byte shutter_mode   = 0;

boolean bulb_mode=false;

// intervalometer time (seconds)
float cam_interval = 1.0;

// max shots
unsigned int cam_max  = 0;

// camera repeat shots
byte cam_repeat = 0;

// last time we entered a repeat cycle or last time we shot if no repeat is set
unsigned long cam_last_tm = 0;

// set speed for the current motor
unsigned int m_speed = 0;

// currently active speed
unsigned int m_cur_speed=0;

//current run time for sms in ms
unsigned long m_sms_tm=0;

//direction of motor
byte m_dir = 0;

// distance (cm) per revolution
float m_diarev =  1.0;

// motor RPMs
float m_rpm    = 9;

// calculated max cpm
float max_cpm =  m_diarev * m_rpm;

// user-configurable min cpm
float min_cpm = 20.0;

// minimumspeed (min cpm->255 scale value) //TODO WHY?
byte min_spd =  (min_cpm / max_cpm) * 255;


// calibration points //TODO unit?
byte motor_spd_cal[2] = {
  2,10};

// maximum sms distance //TODO
unsigned int m_maxsms =  max_cpm * 100;

//SMS or Cont. Mode?
#define MODE_SMS true
#define MODE_CONT false
boolean m_mode=MODE_SMS;

//slow mode settings
unsigned int  m_counter_max_on;
unsigned int  m_counter_max_off;
unsigned int  m_counter_cur;
uint8_t m_pulse_length = 255; //TODO: sinnvollen wert voreintragen

//volatile bool motor_engaged      = false;
volatile bool motor_ran = 0;  //TODO

// motor calibration
//m_cal_array[slot][point][dir]
#define CALPOINT_SMS   0  //Shoot move shoot
#define CALPOINT_PULSE 1  //slow speed pulsing
#define CALPOINT_LOW   2  //cont. speed low
#define CALPOINT_HIGH  3  //cont. speed high

float m_cal_array[3][4][2] = //TODO sinnvolle Werte voreintragen. 
{ 
  {
    {
      1.0,1.0                      }
    ,{
      0.5,0.5                      }
    ,{
      1.0,1.0                      }
    ,{
      2.0,2.0                      }
  }
  ,
   {
    {
      1.0,1.0                      }
    ,{
      0.5,0.5                      }
    ,{
      1.0,1.0                      }
    ,{
      2.0,2.0                      }
  }
  ,
   {
    {
      1.0,1.0                      }
    ,{
      0.5,0.5                      }
    ,{
      1.0,1.0                      }
    ,{
      2.0,2.0                      }
  }
};

//used calibration slot
byte m_slot = 0;

// ramping data
byte m_ramp_in=0;
byte m_ramp_out=0;

// lead-ins
byte m_lead_in  = 0;
byte m_lead_out = 0;


// shots fired
unsigned int shots = 0;

// function types for alt inputs
 enum  __attribute__((packed)) IO_TYPE {
     IO_DISABLED=0, IO_START=1, IO_STOP=2, IO_TOGGLEONOFF=3,IO_TOGGLEDIR=4,IO_EXTINTV=5,IO_OUT_BEFORE=6,IO_OUT_AFTER=7,IO_OUT_BOTH=8};
byte input_type[2] = {IO_DISABLED,IO_DISABLED};
  
unsigned long input_trig_last = 0;


// default alt I/O rising/falling direction
byte altio_dir = FALLING;

//timer globals
void (*timer1_func)();
void (*timer2_func)();
void (*timer3_func)();
volatile byte timer1_s;
volatile byte timer2_s;
unsigned int volatile timer3_ms;
// initialize LCD object
LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() { 
  // setup Pins
  pinModeFast(CAMERA_PIN, OUTPUT);
  pinModeFast(FOCUS_PIN, OUTPUT);
  pinModeFast(IR_PIN, OUTPUT);
  pinModeFast(MOTOR0_P, OUTPUT);
  pinModeFast(MOTOR0_DIR, OUTPUT);
  pinModeFast(BUT_PIN, INPUT);   
  // enable internal pull-up
  digitalWriteFast(BUT_PIN, HIGH);


  //sending custom characters to lcd
   for(byte i=0;i<8;i++){
   memcpy_P(lcd_buf, (uint8_t*) pgm_read_word(&(characters[i])),8);
   delay(50);
   lcd.createChar(i,(uint8_t*)lcd_buf);
   delay(50);
 }
   Serial.begin(115200);

  init_user_interface();

  // check firmware version stored in eeprom
  // will cause eeprom_saved() to return false
  // if version stored in eeprom does not match
  // firmware version.  This automatically clears
  // saved memory after a new firmware load -
  // saving lots of support questions =)

  // did we previously save settings to eeprom?
  if( eeprom_versioning_ok()&& eeprom_saved() ) {
    // restore saved memory
    DEBUG_msg("restore eeprom");
    restore_eeprom_memory();
  }
  else {
    // when wrong version of nothing has been
    // saved, make sure eeprom contains default values
    DEBUG_msg("reset eeprom");
    write_all_eeprom_memory();
  }

  input_last_tm=millis();

  show_home();
  
  #ifdef DEBUG_ON
  /*
    //Output Calibration Data
    for( byte i = 0; i <= 3; i++) {
      Serial.print(i, DEC);
      Serial.print(":");
      for ( byte x = 0; x < 2; x++ ) {
        Serial.print(m_cal_array[0][i][x], 8);
        Serial.print(":");
      }
      Serial.println("");
    }
    */
  DEBUG_msg("start");
  #endif  
  
  initialize_alt_timers();
  alt_io_display_set(lcd_bkl);
  alt_io_motor_set(0);
 
  //XXXX TEST AREA XXXX//
  #ifdef DEBUG_ON
 // memcpy_P(lcd_buf,char_ir,8);

// while(1){}
 
 //XXXX END OF TEST AREA XXXX//
  #endif
}

void loop() {

  // check for signal from gbtimelapse serial command.
  // we check here to prevent queuing commands when stopped
  if( (external_io&EXT_INTV_USB) && gbtl_trigger() == true ) {
    external_io |= EXT_INTV_OK;
  }

  if( S_RUNNING ) {
    // program is running
    main_loop_handler();
  }

  // always check the UI for input or
  // updates

  check_user_interface();    

}

void main_loop_handler() {
  static boolean do_fire        = false;
  static byte    cam_repeated   = 0;
  if( cam_max > 0 && shots >= cam_max) {
    // stop program if max shots exceeded
    stop_executing();
    DEBUG_msg("stop");
    // interrupt further processing      
  }

  // we need to determine if we can shoot the camera
  // by seeing if it is currently being fired, or 
  // is blocked in some way.  After making sure we're
  // not blocked, we check to see if its time to fire the
  // camera
  
  else if( S_IN_DELAY||S_CAM_ENGAGED) {
    // in delay or firing the camera 
    // do nothing
    ;
  }
  else if(S_CAM_CYCLE_COMPLETE) {
    // camera cycle completed
    // clear exposure cycle complete flag
      S_CAM_CYCLE_COMPLETE=false;
      shots++;

      // for ramping motor speed and leads
      // we change speed after shots...
      m_cur_speed=motor_calc_speed_ramp_lead(m_cur_speed, shots);
      motor_set_speed(m_cur_speed);
      
      // is the external trigger to fire?
        if( external_io & (EXT_TRIG_2_AFTER | EXT_TRIG_1_AFTER) ) 
          timer2_set(delay_ext_out, alt_ext_trigger_engage);
      
      // check to see if a post-exposure delay is needed
      if( delay_postexp > 0 ) {
      //start post exposure delay
      S_IN_DELAY=true;
      timer2_set(delay_postexp,clear_delay);
      }
        
  }
  
  else if(external_io & (EXT_INTV_1|EXT_INTV_2|EXT_INTV_USB) ) {
    // external intervalometer is engaged

    if( external_io & EXT_INTV_OK ) {
      // external intervalometer has triggered

      // clear out ok to fire flag
      external_io &= ~EXT_INTV_OK;      
      do_fire = true;
    }
  }
  
  else if( cam_last_tm < millis() - (cam_interval * 1000) ) {
    // internal intervalometer triggers
    do_fire = true;
  }
  


  if( do_fire == true ) {
    // we've had a fire camera event
    // we are not in an delay
    
    if(cam_repeated == 0 ){
      //first shot of repeat cylce
      cam_last_tm  = millis();
      // is the external trigger to fire before shot and hasn't been setup yet?
      if ((external_io & (EXT_TRIG_1_BEFORE|EXT_TRIG_2_BEFORE))&&!S_EXT_TRIG_SETUP)
      { 
        //calculate when the trigger is to be enabled
        //shot will happen in (delay_preexp+delay_focus+100),
        //trigger should be enabled delay_ext_out ms before that.
        unsigned int calc_delay=(delay_preexp+delay_focus+100)-delay_ext_out;
        timer2_set(calc_delay,alt_ext_trigger_engage);
        S_EXT_TRIG_SETUP=true;
      }
      //start delays for first exposue  
      if (S_DELAYS_DONE==false) {
        timer1_set(delay_preexp,focus_camera);
        S_IN_DELAY=true;
      }
    }
    else{ 
      //in repeat cycle
      if (S_DELAYS_DONE==false){
        timer1_set(delay_repeat,focus_camera);
        S_IN_DELAY=true;
      }
      
      if (S_IN_DELAY==false&&S_DELAYS_DONE==true){
        //delays are done
        fire_camera(exp_tm);
        
        // setup for next call 
  
        // deal with camera repeat actions
        if( cam_repeat == 0 || (cam_repeat > 0  && cam_repeated >= cam_repeat) ) {
            //no more repeats
          S_CAM_CYCLE_COMPLETE=true;
          do_fire = false;
          cam_repeated = 0;
        }
        else if( cam_repeat > 0 ) {
        cam_repeated++;
        }
      }
    }
  }  
}


void start_executing() {
  // starts program execution

  // clear out external interval flag in case it was
  // set while stopped.
  external_io&= ~EXT_INTV_OK;

 // turn on main_handler
  S_RUNNING=true; 
  shots = 0;
  
}

void stop_executing() {
  //stops program execution and motors
  S_RUNNING=false;
  S_MOT_RUNNING=false;
  motor_set_speed(0);
}

boolean gbtl_trigger() {
  if( Serial.available() > 0 ) {
    char thsChar = Serial.read();
    if( thsChar == 'T' ) return true;
    }
  return false;
  }

