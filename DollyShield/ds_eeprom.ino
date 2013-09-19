/* 
 
 El Cheapo Dollyshield -  EEPROM Functions
 modified Version of Dynamic Perception LLC's DollyShield
 (c) 2010-2011 C.A. Church / Dynamic Perception LLC ds_eeprom.ino
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

 ========================================
 EEPROM write/read functions
 ========================================
 
 */


/* 
 
 *******************************
 Compiler does Mapping of Data Positions in EEPROM memory
 *******************************
*/ //TODO

byte EEMEM E_lcd_bkl;
boolean EEMEM E_blank_lcd;
byte EEMEM E_lcd_dim_tm;
boolean EEMEM E_ui_invdir;;
boolean EEMEM E_ui_motor_display;
byte EEMEM E_external_io;
uint32_t EEMEM E_exp_tm;
uint16_t EEMEM E_delay_postexp;
uint16_t EEMEM E_delay_preexp;
uint16_t EEMEM E_delay_ext_in ;
uint16_t EEMEM E_delay_ext_out;
uint16_t EEMEM E_length_ext_out;
uint16_t EEMEM E_delay_focus;
uint16_t EEMEM E_delay_repeat;
byte EEMEM E_shutter_mode;
boolean EEMEM E_bulb_mode;
float EEMEM E_cam_interval:
uint16_t EEMEM E_cam_max;
byte EEMEM E_cam_repeat;
uint16_t EEMEM E_m_speed;
byte EEMEM E_m_dir;
float EEMEM E_m_diarev;
float EEMEM E_m_rpm;
float EEMEM E_max_cpm;
float EEMEM E_min_cpm;
byte EEMEM E_min_spd;
byte EEMEM E_motor_spd_cal[2];
uint16_t EEMEM E_m_maxsms;
boolean EEMEM E_m_mode=MODE_SMS;
uint8_t m_pulse_length;
float EEMEM E_m_cal_array[3][4][2];
byte EEMEM E_m_angle;
boolean EEMEM E_m_cal_done;
byte EEMEM E_m_ramp_in;
byte EEMEM E_m_ramp_out;
byte EEMEM E_m_lead_in;
byte EEMEM E_m_lead_out;
byte EEMEM E_input_type[2];
byte EEMEM E_altio_dir;



#define EEPROM_IS_SAVED 170


boolean eeprom_saved() {

 return (eeprom_read_byte(&E_eeprom_saved)==EEPROM_IS_SAVED)
}

void eeprom_saved( boolean saved ) {
  if (saved!=(eeprom_saved==EEPROM_IS_SAVED){
    //saved status changed 
  eeprom_saved=saved?EEPROM_IS_SAVED:0 ;
  eeprom_save(E_eeprom_saved,eeprom_saved);
  }

}




// One can ask why I didn't use the templates from http://www.arduino.cc/playground/Code/EEPROMWriteAnything
// The primary reason here is that we're going to be calling these functions OFTEN, and I _really_ don't 
// want the templates getting inlined _everywhere_, what a mess!  So, rather than be slick, let's just declare
// what we mean, and do it once - forget about the overhead of the function call, and worry more about
// flash and stack abuse 


void eeprom_save( void * dst, const void * src, size_t  n)  {
  eeprom_write_block(dst, src, n);
  eeprom_saved(true);  
}

void eeprom_save( uint16_t& pos, uint16_t& val ) {
  eeprom_write_word(&pos,val); 
    // indicate that memory has been saved
  eeprom_saved(true); 
}

void eeprom_save( int pos,  int& val ) {
  eeprom_write_word(&pos,(uint16_t)val); 
    // indicate that memory has been saved
  eeprom_saved(true);    
}

void eeprom_save( byte& pos, byte& val ) {
  eeprom_write_byte(&pos,val); 
    // indicate that memory has been saved
  eeprom_saved(true);   
}

void eeprom_save( boolean& pos, boolean& val ) {
  eeprom_write_byte(&pos,(byte)val); 
    // indicate that memory has been saved
  eeprom_saved(true);   
}

void eeprom_save( float& pos, float& val ) {
  eeprom_write_float(&pos,val); 
    // indicate that memory has been saved
  eeprom_saved(true);  
}

void eeprom_save( unsigned long& pos, unsigned long& val ) {  
  eeprom_write_dword(&pos,val); 
  // indicate that memory has been saved
  eeprom_saved(true);
}





// read functions


void eeprom_load( void * dst, const void * src, size_t  n)  {
  eeprom_read_block(dst, src, n); 
}

void eeprom_load( uint16_t& pos, uint16_t& val ) {
  val=eeprom_read_word(&pos);
}


void eeprom_load( int& pos, int& val ) {
  val=(int)eeprom_read_word(&pos);
}

void eeprom_load( unsigned long& pos, unsigned long& val ) {
  val=eeprom_read_dword(&pos);
}

void eeprom_load( uint8_t& pos, uint8_t& val ) {
  val=eeprom_read_byte(&pos);
}
void eeprom_load( boolean& pos, boolean& val ) {
  val=(boolean) eeprom_read_byte(&pos);
}
void eeprom_load(float& pos, float& val ) {
  val=eeprom_read_float(&pos);
}

void write_all_eeprom_memory() {
/*
  // write default values into eeprom
  eeprom_write(EEPROM_TODO, delay_focus);
  eeprom_write(EEPROM_TODO, delay_postexp);
  * 
  eeprom_write(EEPROM_TODO, focus_shutter);

  eeprom_write(EEPROM_TODO, cam_max);
  eeprom_write(EEPROM_TODO, m_diarev);
  eeprom_write(EEPROM_TODO, max_cpm);
  eeprom_write(EEPROM_TODO, m_rpm);
  eeprom_write(EEPROM_TODO, min_cpm);
  eeprom_write(EEPROM_TODO, min_spd);
  eeprom_write(EEPROM_TODO, m_min_pulse);
  eeprom_write(EEPROM_TODO, altio_dir);
  eeprom_write(EEPROM_TODO, ui_motor_display);
  eeprom_write(EEPROM_TODO, motor_sl_mod);
  eeprom_write(EEPROM_TODO, lcd_dim_tm);
  eeprom_write(EEPROM_TODO, blank_lcd);
  eeprom_write(EEPROM_TODO, m_ramp_set);
  eeprom_write(EEPROM_TODO, m_maxsms);
  eeprom_write(EEPROM_TODO, cam_interval);

  // handle m_cal_array in a sane manner
  // float m_cal_array[3][4][2] 
  // 3 * 4 * 2 * 4 = 96

  byte* p = (byte*)(void*)&m_cal_array;
  eeprom_write(EEPROM_TODO, *p, (3*4*2*4));

  eeprom_write(EEPROM_TODO, input_type[0]);
  eeprom_write(EEPROM_TODO, input_type[1]);
  eeprom_write(EEPROM_TODO, ui_is_metric);
  eeprom_write(EEPROM_TODO, m_lead_in);
  eeprom_write(EEPROM_TODO, m_lead_out);
  eeprom_write(EEPROM_TODO, motor_spd_cal);
  eeprom_write(EEPROM_TODO, m_cal_constant);
  eeprom_write(EEPROM_TODO, cam_repeat);
  eeprom_write(EEPROM_TODO, delay_repeat);

  eeprom_write(EEPROM_TODO, ext_trig_pre_delay);
  eeprom_write(EEPROM_TODO, ext_trig_pst_delay);
  eeprom_write(EEPROM_TODO, exp_tm);
  eeprom_write(EEPROM_TODO, gb_enabled);
  eeprom_write(EEPROM_TODO, ui_invdir);
  eeprom_write(EEPROM_TODO, lcd_bkl);
  eeprom_write(EEPROM_TODO, ir_remote);
  */
}


// restore memory

void restore_eeprom_memory() {
/*
  // read eeprom stored values back into RAM

  eeprom_read(3, delay_focus);
  eeprom_read(5, delay_postexp);
  eeprom_read(7, focus_shutter);

  eeprom_read(10, cam_max);
  eeprom_read(16, m_diarev[0]);
  eeprom_read(24, max_cpm);
  eeprom_read(32, m_rpm[0]);
  eeprom_read(40, min_cpm[0]);
  eeprom_read(48, min_spd[0]);
  eeprom_read(50, m_min_pulse[0]);
  eeprom_read(52, altio_dir);

  eeprom_read(56, ui_motor_display);
  eeprom_read(57, motor_sl_mod);
  eeprom_read(58, lcd_dim_tm);
  eeprom_read(60, blank_lcd);
  eeprom_read(61, m_ramp_set[0]);
  eeprom_read(63, m_maxsms[0]);
  eeprom_read(67, cam_interval);

  
  // handle m_cal_array in a sane manner
  // float m_cal_array[3][4][2] 
  // 3 * 4 * 2 * 4 = 96

  byte* p = (byte*)(void*)&m_cal_array;
  eeprom_read(71, *p, (3*4*2*4));

  eeprom_read(217, input_type[0]);
  eeprom_read(218, input_type[1]);

  eeprom_read(219, ui_is_metric);

  eeprom_read(229, m_lead_in[0]);
  eeprom_read(233, m_lead_out[0]);

  eeprom_read(237, motor_spd_cal[0]);

  eeprom_read(239, m_cal_constant[0]);

  eeprom_read(249, cam_repeat);
  eeprom_read(250, delay_repeat);

  eeprom_read(252, ext_trig_pre_delay);
  eeprom_read(256, ext_trig_pst_delay);

  eeprom_read(260, exp_tm); // moved from position 1

  eeprom_read(264, gb_enabled);
  eeprom_read(265, ui_invdir);
  eeprom_read(266, lcd_bkl);
  eeprom_read(267, ir_remote);
  // handle restoring alt input states

  if( input_type[0] != 0 )
    altio_connect(0,input_type[0]);

  if( input_type[1] != 0 )
    altio_connect(1,input_type[1]);

  // set lcd backlight to saved value
  ui_set_backlight(lcd_bkl);
*/
}



boolean eeprom_versioning_ok() {
  // determine if eeprom version is correct 
  // so we can automatically flush saved memory 
  // when a new firmware is loaded 
  //TODO
  unsigned int eeprom_ver = 0;
  eeprom_read(247, eeprom_ver);

  // wipe out any saved eeprom settings
  return( eeprom_ver == FIRMWARE_VERSION );


}


