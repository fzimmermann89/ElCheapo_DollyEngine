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
 (E_varname is the position of varname)
 *******************************
*/

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
float EEMEM E_cam_interval;
uint16_t EEMEM E_cam_max;
byte EEMEM E_cam_repeat;
uint16_t EEMEM E_m_speed;
byte EEMEM E_m_dir;
float EEMEM E_m_diarev;
float EEMEM E_m_rpm;
float EEMEM E_max_cpm;
float EEMEM E_min_cpm;
byte EEMEM E_min_spd;
byte EEMEM E_motor_spd_cal0;
byte EEMEM E_motor_spd_cal1;
uint16_t EEMEM E_m_maxsms;
boolean EEMEM E_m_mode;
uint8_t EEMEM E_m_pulse_length;
float EEMEM E_m_cal_array[3][4][2];
byte EEMEM E_m_slot;
byte EEMEM E_m_ramp_in;
byte EEMEM E_m_ramp_out;
byte EEMEM E_m_lead_in;
byte EEMEM E_m_lead_out;
byte EEMEM E_input_type0;
byte EEMEM E_input_type1;
byte EEMEM E_altio_dir;
byte EEMEM E_eeprom_saved;
uint8_t EEMEM E_eeprom_ver;

//Magic Value
#define EEPROM_IS_SAVED 170



//Functions for keeping track of changes and version

boolean eeprom_saved() {
  byte test=eeprom_read_byte(&E_eeprom_saved);
  DEBUG_var("issaved:",test);
 return (test==EEPROM_IS_SAVED);
}

void eeprom_saved( boolean saved ) {
  static byte _eeprom_saved=eeprom_read_byte(&E_eeprom_saved);
  DEBUG_var("_saved:",_eeprom_saved);
    DEBUG_var("saved:",saved);

  if (saved!=(_eeprom_saved==EEPROM_IS_SAVED)){
   DEBUG_msg("status changed");
  _eeprom_saved=(saved==true?EEPROM_IS_SAVED:false);
    DEBUG_var("new_saved:",_eeprom_saved);
  eeprom_write_byte(&E_eeprom_saved,_eeprom_saved);
  }
}

boolean eeprom_versioning_ok() {
  // determine if eeprom version is correct 
  // so we can automatically flush saved memory 
  // when a new firmware is loaded 
  byte eeprom_ver = 0;
  eeprom_load(E_eeprom_ver, eeprom_ver);
DEBUG_var("ever:",eeprom_ver);
byte test=FIRMWARE_VERSION;
DEBUG_var("sver:",test);
  return( eeprom_ver == FIRMWARE_VERSION );
}



//Save functions that check before writing and set the eeprom_saved flag.

void eeprom_save (void * dst ,const void * src, size_t n)  {
  //this one doesn't check...
  eeprom_write_block (src, dst, n);
  eeprom_saved(true);  
}

void eeprom_save( uint16_t& pos, uint16_t& val ) {
  if (eeprom_read_word(&pos)!=val) eeprom_write_word(&pos,val); 
    // indicate that memory has been saved
  eeprom_saved(true); 
}

void eeprom_save( int& pos,  int& val ) {
    if (eeprom_read_word((uint16_t*)&pos)!=(uint16_t)val) eeprom_write_word((uint16_t*)&pos,(uint16_t)val); 
    // indicate that memory has been saved
  eeprom_saved(true);    
}

void eeprom_save( byte& pos, byte& val ) {
    if (eeprom_read_byte(&pos)!=val) eeprom_write_byte(&pos,val); 
    // indicate that memory has been saved
  eeprom_saved(true);   
}

void eeprom_save( float& pos, float& val ) {
  //eeprom_write_float() not in avrlibc version used by Arduino!
  //check, write only if changed
  float oldvalue;
  eeprom_read_block((void*)&oldvalue, (const void*)&pos, sizeof(float));
  if (oldvalue!=val) eeprom_write_block(&val,&pos,  sizeof(float)); 
  // indicate that memory has been saved
  eeprom_saved(true);  
}

void eeprom_save( unsigned long& pos, unsigned long& val ) {  
  
   if (eeprom_read_dword(&pos)!=val)  eeprom_write_dword(&pos,val); 
  // indicate that memory has been saved
  eeprom_saved(true);
}




// Read Functions

void eeprom_load(const void * src, void * dst, size_t  n)  {
  eeprom_read_block(dst, src, n); 
}

void eeprom_load( uint16_t& pos, uint16_t& val ) {
  val=eeprom_read_word(&pos);
}

void eeprom_load( int& pos, int& val ){ 
  val=(int)eeprom_read_word((uint16_t*) (void*) &pos);
}

void eeprom_load( unsigned long& pos, unsigned long& val ) {
  val=eeprom_read_dword(&pos);
}

void eeprom_load( uint8_t& pos, uint8_t& val ) {
  val=eeprom_read_byte(&pos);
}

void eeprom_load(float& pos, float& val ) {
  //eeprom_read_float not in avrlibc version used by Arduino!
  eeprom_read_block(&val, &pos, sizeof(float));   
}



//Write all Values to EEPROM

void write_all_eeprom_memory() {
  eeprom_save(E_delay_focus, delay_focus);
  eeprom_save(E_delay_postexp, delay_postexp);
  eeprom_save(E_delay_preexp, delay_postexp);
  eeprom_save(E_delay_repeat, delay_repeat);
  eeprom_save(E_delay_ext_in, delay_ext_in);
  eeprom_save(E_delay_ext_out, delay_ext_out);
  eeprom_save(E_length_ext_out,length_ext_out);
  eeprom_save(E_altio_dir, altio_dir);
  eeprom_save(E_m_diarev, m_diarev);
  eeprom_save(E_m_rpm, m_rpm);
  eeprom_save(E_max_cpm, max_cpm);
  eeprom_save(E_min_cpm, min_cpm);
  eeprom_save(E_min_spd, min_spd);
  eeprom_save(E_m_pulse_length, m_pulse_length);
  eeprom_save(E_m_ramp_in, m_ramp_in);
  eeprom_save(E_m_ramp_out, m_ramp_out);
  eeprom_save(E_m_lead_in, m_lead_out);
  eeprom_save(E_m_lead_out, m_lead_out);
  eeprom_save(E_shutter_mode,shutter_mode );
  eeprom_save(E_bulb_mode, bulb_mode);
  eeprom_save(E_m_speed,m_speed );
  eeprom_save(E_m_mode,m_mode );
  eeprom_save(E_ui_invdir, ui_invdir);
  eeprom_save(E_ui_motor_display, ui_motor_display);
  eeprom_save(E_lcd_dim_tm, lcd_dim_tm);
  eeprom_save(E_blank_lcd, blank_lcd);
  eeprom_save(E_lcd_bkl,lcd_bkl);  
  eeprom_save(E_m_ramp_in, m_ramp_in);
  eeprom_save(E_m_ramp_out, m_ramp_out);
  eeprom_save(E_m_lead_in, m_lead_out);
  eeprom_save(E_cam_interval, cam_interval);
  eeprom_save(E_cam_max, cam_max);
  eeprom_save(E_exp_tm, exp_tm);
  eeprom_save(E_cam_repeat, cam_repeat);
  eeprom_save(E_input_type0, input_type[0]);
  eeprom_save(E_input_type1, input_type[1]);
  eeprom_save(E_motor_spd_cal0, motor_spd_cal[0]);
  eeprom_save(E_motor_spd_cal1, motor_spd_cal[1]);
  eeprom_save(E_m_cal_array, m_cal_array, sizeof(m_cal_array));
  eeprom_save(E_eeprom_ver,const_cast<uint8_t&>(FIRMWARE_VERSION));//
}



// Restore memory from EEPROM

void restore_eeprom_memory() {
  eeprom_load(E_delay_focus, delay_focus);
  eeprom_load(E_delay_postexp, delay_postexp);
  eeprom_load(E_delay_preexp, delay_postexp);
  eeprom_load(E_delay_repeat, delay_repeat);
  eeprom_load(E_delay_ext_in, delay_ext_in);
  eeprom_load(E_delay_ext_out, delay_ext_out);
  eeprom_load(E_length_ext_out,length_ext_out);
  eeprom_load(E_altio_dir, altio_dir);
  eeprom_load(E_m_diarev, m_diarev);
  eeprom_load(E_m_rpm, m_rpm);
  eeprom_load(E_max_cpm, max_cpm);
  eeprom_load(E_min_cpm, min_cpm);
  eeprom_load(E_min_spd, min_spd);
  eeprom_load(E_m_pulse_length, m_pulse_length);
  eeprom_load(E_m_ramp_in, m_ramp_in);
  eeprom_load(E_m_ramp_out, m_ramp_out);
  eeprom_load(E_m_lead_in, m_lead_out);
  eeprom_load(E_m_lead_out, m_lead_out);
  eeprom_load(E_shutter_mode,shutter_mode );
  eeprom_load(E_bulb_mode, bulb_mode);
  eeprom_load(E_m_speed,m_speed );
  eeprom_load(E_m_mode,m_mode );
  eeprom_load(E_ui_invdir, ui_invdir);
  eeprom_load(E_ui_motor_display, ui_motor_display);
  eeprom_load(E_lcd_dim_tm, lcd_dim_tm);
  eeprom_load(E_blank_lcd, blank_lcd);
  eeprom_load(E_lcd_bkl,lcd_bkl);  
  eeprom_load(E_m_ramp_in, m_ramp_in);
  eeprom_load(E_m_ramp_out, m_ramp_out);
  eeprom_load(E_m_lead_in, m_lead_out);
  eeprom_load(E_cam_interval, cam_interval);
  eeprom_load(E_cam_max, cam_max);
  eeprom_load(E_exp_tm, exp_tm);
  eeprom_load(E_cam_repeat, cam_repeat);
  eeprom_load(E_input_type0, input_type[0]);
  eeprom_load(E_input_type1, input_type[1]);
  eeprom_load(E_motor_spd_cal0, motor_spd_cal[0]);
  eeprom_load(E_motor_spd_cal1, motor_spd_cal[1]);
  eeprom_load(E_m_cal_array, m_cal_array, sizeof(m_cal_array)); //TODO
}






