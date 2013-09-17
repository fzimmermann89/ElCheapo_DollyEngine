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
 Mapping of Data Positions in EEPROM memory
 *******************************
 
 (position count starts at zero)
 //TODO
 flash enabled   = 0
 (was exp_tm)    = 1-2*
 delay_focus    = 3-4
 delay_postexp   = 5-6
 focus_shutter   = 7
 = 8-9
 cam_max         = 10-11
 m_speeds	     = 12-13
				 = 14-15
 m_diarev	     = 16-19
				 = 20-23
 max_cpm	     = 24-27
				 = 28-31
 m_rpm        	 = 32-35
				 = 36-39
 min_cpm	     = 40-43
				 = 44-47
 min_spd      	 = 48
				 = 49
 m_min_pulse[0]  = 50
 m_min_pulse[0]  = 51
 altio_dir       = 52
				 = 53
				 = 54
				 = 55 
 ui_motor_display= 56
 motor_sl_mod    = 57
 lcd_dim_tm      = 58-59
 blank_lcd       = 60
 m_ramp_set[0]   = 61
 m_ramp_set[0]   = 62
 m_maxsms[0]     = 63-64
 m_maxsms[1]     = 65-66
 cam_interval    = 67-70
 m_cal_array[]   = 71-214
 m_angle[0]      = 215
 m_angle[1]      = 216 
 input_type[0]   = 217
 input_type[1]   = 218
 ui_is_metric    = 219
 merlin_enable   = 220
 merlin_man_spd[0] = 221-224
 merlin_man_spd[1] = 225-228
 m_lead_in[0]    = 229-230
 m_lead_in[1]    = 231-232
 m_lead_out[0]   = 233-234
 m_lead_out[1]   = 235-236
 motor_spd_cal[0] = 237
 motor_spd_cal[1] = 238
 m_cal_constant[0] = 239-242
 m_cal_constant[1] = 243-246
 firmware_version  = 247-248
 cam_repeat     = 249
 delay_repeat    = 250-251
 ext_trig_pre_dly = 252-255
 ext_trig_pst_dly = 256-259
 exp_tm         = 260-263
 gb_enabled     = 264
 ui_invdir      = 265
 cur_bkl        = 266
 use_ir         = 267
 */

#define EEPROM_IS_SAVED 170


boolean eeprom_saved() {

  // read eeprom saved status
  // is Magic Value in position 0?
  byte saved = EEPROM.read(0);
  return(saved==EEPROM_IS_SAVED );
}

void eeprom_saved( boolean saved ) {
  // set eeprom saved status
  // write Magic Value to position 0
  EEPROM.write(0, EEPROM_IS_SAVED);
}





// One can ask why I didn't use the templates from http://www.arduino.cc/playground/Code/EEPROMWriteAnything
// The primary reason here is that we're going to be calling these functions OFTEN, and I _really_ don't 
// want the templates getting inlined _everywhere_, what a mess!  So, rather than be slick, let's just declare
// what we mean, and do it once - forget about the overhead of the function call, and worry more about
// flash and stack abuse 


void eeprom_write( int pos, byte& val, byte len ) {
  byte* p = (byte*)(void*)&val;
  for( byte i = 0; i < len; i++ )
    EEPROM.write(pos++, *p++);    

  // indicate that memory has been saved
  eeprom_saved(true);

}

void eeprom_write( int pos, unsigned int& val ) {
  byte* p = (byte*)(void*)&val;   
  eeprom_write(pos, *p, sizeof(int));  
}

void eeprom_write( int pos, unsigned long& val ) {
  byte* p = (byte*)(void*)&val;   
  eeprom_write(pos, *p, sizeof(long));    
}

void eeprom_write( int pos, float& val ) {
  byte* p = (byte*)(void*)&val;   
  eeprom_write(pos, *p, sizeof(float));    
}

void eeprom_write( int pos, byte& val ) {  
  EEPROM.write(pos, val);
  // indicate that memory has been saved
  eeprom_saved(true);
}





// read functions

void eeprom_read( int pos, byte& val, byte len ) {
  byte* p = (byte*)(void*)&val;
  for(byte i = 0; i < len; i++) 
    *p++ = EEPROM.read(pos++);
}

void eeprom_read( int pos, byte& val ) {
  val = EEPROM.read(pos);
}


void eeprom_read( int pos, int& val ) {
  byte* p = (byte*)(void*)&val;
  eeprom_read(pos, *p, sizeof(int));
}

void eeprom_read( int pos, unsigned int& val ) {

  byte* p = (byte*)(void*)&val;
  eeprom_read(pos, *p, sizeof(int));

}

void eeprom_read( int pos, unsigned long& val ) {

  byte* p = (byte*)(void*)&val;
  eeprom_read(pos, *p, sizeof(long));

}

void eeprom_read( int pos, float& val ) {

  byte* p = (byte*)(void*)&val;
  eeprom_read(pos, *p, sizeof(float));

}

void write_all_eeprom_memory() {
/*
  // write default values into eeprom
  eeprom_write(EEPROM_TODO, delay_focus);
  eeprom_write(EEPROM_TODO, delay_postexp);
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
  eeprom_write(EEPROM_TODO, cur_bkl);
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
  eeprom_read(266, cur_bkl);
  eeprom_read(267, ir_remote);
  // handle restoring alt input states

  if( input_type[0] != 0 )
    altio_connect(0,input_type[0]);

  if( input_type[1] != 0 )
    altio_connect(1,input_type[1]);

  // set lcd backlight to saved value
  ui_set_backlight(cur_bkl);
*/
}



boolean eeprom_versioning_ok() {
  // determine if eeprom version is correct 
  // so we can automatically flush saved memory 
  // when a new firmware is loaded 

  unsigned int eeprom_ver = 0;
  eeprom_read(247, eeprom_ver);

  // wipe out any saved eeprom settings
  return( eeprom_ver == FIRMWARE_VERSION );


}


