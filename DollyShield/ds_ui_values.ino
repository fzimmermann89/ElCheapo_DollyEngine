/* 
 
 El Cheapo Dollyshield - UI Values
 modified Version of Dynamic Perception LLC's DollyShield ds_ui_values.ino
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

 ========================================
 UI Value Handling/Lookup functions
 ========================================
 
 */


void get_value( byte menu, byte pos, boolean read_save ) {

  // find the correct value to display for the current menu item
  // calls the necessary related function to handle value display/setting
  if( pos > max_menu[menu] )
    return;

  //set as non-floating point/bool by default
  ui_type = INPUT_ONOFF;

  switch(menu) {
  case 1:
    get_manual_select(pos);
    break;
  case 2:    
    get_m_set(pos, read_save);
    break;
  case 3:    
    get_cam_set(pos, read_save);
    break;
  case 4:
    get_global_set(pos, read_save);
    break;
  case 5:
    get_m_adv_set(pos,read_save);
    break;

  }



}


void move_val(boolean dir) {
  // increase or decrease input value
  
  if( ui_type==INPUT_FLOAT) {
    // float type


    // how much to add/remove each time?

    float mod = ui_float_tenths ? 0.1 : 0.01;
    mod *= (float) inp_val_mult;

    // floating point input
    if( dir == true ) {
    // increase value till max
    if( cur_inp_float > cur_inp_float_max-mod ) {
        cur_inp_float = cur_inp_float_max;
      }
      else {
        cur_inp_float += mod;
      }
    }
    else {
      //decrease value till min
      if( cur_inp_float < cur_inp_float_min+mod ) {
        cur_inp_float = cur_inp_float_min;
      }
      else {
        cur_inp_float -= mod;
      }

    }

  }
  else if( ui_type==INPUT_ONOFF ||ui_type==INPUT_LTRT ||ui_type==INPUT_CMPCT || ui_type==INPUT_CONTSMS ||ui_type==INPUT_OKCANCEL ) {
    // any boolean type
    cur_inp_bool = ! cur_inp_bool;
  }
  else {
    // unsigned int type
    
     // ceiling on certain special values values
    if (ui_type==INPUT_SLOT ) {
      cur_inp_int_max = 2;
      cur_inp_int_min = 0;
      inp_val_mult    = 1;
    }
    else if (ui_type==INPUT_IO ) {
      cur_inp_int_max = 8;
      cur_inp_int_min = 0;
      inp_val_mult    = 1;
    }
    else if (ui_type==INPUT_SHUTTER ) {
    cur_inp_int_max = 3;
    cur_inp_int_min = 0;
    inp_val_mult    = 1;
    }
    
    
    
    
    unsigned int mod = (1 * inp_val_mult);
    // long input
    if( dir == true ) {
    // increase value till max
    if( cur_inp_int > cur_inp_int_max-mod ) {
        cur_inp_int = cur_inp_int_max;
      }
      else {
        cur_inp_int += mod;
      }
    }
    else {
      //decrease value till min
      if( cur_inp_int < cur_inp_int_min+mod ) {
        cur_inp_int = cur_inp_int_min;
      }
      else {
        cur_inp_int -= mod;
      }

    }



  }
}




void get_m_set( byte pos, boolean read_save) {
  // set motor configurable values

  switch(pos) { 
  case 0:
     // set Movement Mode (SMS/CONT)
    ui_type=INPUT_CONTSMS;
    if( read_save == true ) {
      m_mode = cur_inp_bool;
      eeprom_save(E_m_mode, m_mode);
    }
    cur_inp_bool = m_mode;
    break;
  case 1:
    //display mode
     ui_type=INPUT_CMPCT;
    if( read_save == true ) {
      ui_motor_display = cur_inp_bool;
      eeprom_save(E_ui_motor_display, ui_motor_display);
    }
    cur_inp_bool = ui_motor_display;
    break;
  case 2:
    // set ramp in value
    ui_type = INPUT_UINT;
    cur_inp_int_max=255;
    cur_inp_int_min=0;
    if( read_save == true ) {
      m_ramp_in=cur_inp_int;         
      eeprom_save(E_m_ramp_in, m_ramp_in);
    }
    cur_inp_int = m_ramp_in;
    break;

  case 3:
    // set ramp out value
    ui_type = INPUT_UINT;
    cur_inp_int_max=255;
    cur_inp_int_min=0;
    if( read_save == true ) {
      m_ramp_out=cur_inp_int;         
      eeprom_save(E_m_ramp_out, m_ramp_out);
    }

    cur_inp_int = m_ramp_out;
    break;

  case 4:
    // set lead-in value
    ui_type = INPUT_UINT;
    cur_inp_int_max=255;
    cur_inp_int_min=0;
    if( read_save == true ) {
      m_lead_in = cur_inp_int;
      eeprom_save(E_m_lead_in, m_lead_in);
    }

    cur_inp_int = m_lead_in;
    break;

  case 5:
    // set lead-out value
    ui_type = INPUT_UINT;
    cur_inp_int_max=255;
    cur_inp_int_min=0;
    if( read_save == true ) {
      m_lead_out = cur_inp_int;
      eeprom_save(E_m_lead_out, m_lead_out);
    }

    cur_inp_int = m_lead_out;
    break;

  case 6: 
    // cal setting
    ui_type=INPUT_SLOT;

    if( read_save == true ) {
      m_slot = cur_inp_int;
      eeprom_save(E_m_slot, m_slot);
    }

    cur_inp_int = m_slot;
    break;
  }

}

void get_m_adv_set( byte pos, boolean read_save){
  switch (pos){
  case 0:
    // calibrate motor
    get_calibrate_select(0);
    break;

  case 1:
    // set rpm
    ui_type = INPUT_FLOAT;
    cur_inp_float_max=100.0;
    cur_inp_float_min=0;

    if( read_save == true ) {
      m_rpm = cur_inp_float;
      motor_update_dist(m_rpm, m_diarev);
      eeprom_save(E_m_rpm, m_rpm);
    }

    cur_inp_float = m_rpm;
    break;

  case 2:
    // distance per revolution
    ui_type=INPUT_FLOAT;
    cur_inp_float_max=100.0;
    cur_inp_float_min=0;
    if( read_save == true ) {
      m_diarev = cur_inp_float;
      motor_update_dist(m_rpm, m_diarev);
      eeprom_save(E_m_diarev, m_diarev);
    }
    cur_inp_float = m_diarev;
    break;

  case 3:
    ui_type=INPUT_FLOAT;
    cur_inp_float_max=max_cpm;
    cur_inp_float_min=0;
    if( read_save == true ) {
      min_cpm = cur_inp_float;
      min_spd = 255 * ( min_cpm / max_cpm ); //TODO

      eeprom_save(E_min_cpm, min_cpm);
      eeprom_save(E_min_spd, min_spd);
    } 
    cur_inp_float = min_cpm;
    break;

  case 4:
    // pulse power
    ui_type=INPUT_UINT;
    cur_inp_int_max=255;
    cur_inp_int_min=0;
    if( read_save == true ) {
      m_pulse_length = cur_inp_int;
      eeprom_save(E_m_pulse_length, m_pulse_length);
    }
    cur_inp_int = m_pulse_length;
    break;


  case 5:
    // low calibration spd
    ui_type=INPUT_FLOAT;
    cur_inp_float_max=max_cpm;
    cur_inp_float_min=min_cpm;
    if( read_save == true ) {
      motor_spd_cal[0] = calc_speed(cur_inp_float);
      eeprom_save(E_motor_spd_cal0, motor_spd_cal[0]);
    }
    cur_inp_float = calc_cpm(motor_spd_cal[0]);
    break;

  case 6:
    // high calibration spd
    ui_type=INPUT_FLOAT;
    cur_inp_float_max=max_cpm;
    cur_inp_float_min=min_cpm;
    if( read_save == true ) {
      motor_spd_cal[1] = calc_speed(cur_inp_float);
      eeprom_save(E_motor_spd_cal1, motor_spd_cal[1]);
    }
    cur_inp_float = calc_cpm(motor_spd_cal[1]);
    break;

  }

}


void get_cam_set( byte pos, boolean read_save ) {

  // reset this flag
  ui_float_tenths = false;


  switch(pos) {
  case 0:
    // interval timer
    ui_type=INPUT_FLOAT;
    ui_float_tenths = true;
    cur_inp_float_min=calc_total_cam_tm()/1000;
    cur_inp_float_max=UINT_MAX;
    if( read_save == true ) { 
      cam_interval = cur_inp_float;
      eeprom_save(E_cam_interval, cam_interval);
    }
    cur_inp_float = cam_interval;
    break;

  case 1:
    // max shots
    ui_type=INPUT_UINT;
    cur_inp_int_max=UINT_MAX;
    cur_inp_int_min=calc_min_cam_max();

    if( read_save == true ) {
      cam_max = cur_inp_int;
      eeprom_save(E_cam_max, cam_max);
    }
    cur_inp_int = cam_max;
    break;

  case 2:
    // exposure time
    ui_type=INPUT_UINT;
    cur_inp_int_min=0;
    cur_inp_int_max=UINT_MAX;  //TODO what about bulb?
    if( read_save == true ) { 
      exp_tm = cur_inp_int;
      eeprom_save(E_exp_tm, exp_tm);
    }
    cur_inp_int = exp_tm;
    break;    

  case 3:
    //Shutter Type
    ui_type=INPUT_SHUTTER;
    if( read_save == true ) { 
      shutter_mode = cur_inp_int;
      eeprom_save(E_shutter_mode, shutter_mode);
    }
    cur_inp_int = shutter_mode;
    break;

  case 4:
    //bulb mode
    ui_type=INPUT_ONOFF;
    if( read_save == true ) {
      bulb_mode = cur_inp_bool;
      eeprom_save(E_bulb_mode, bulb_mode);
    }

    cur_inp_bool = bulb_mode;
    break;

  case 5:
    // camera repeat value
    ui_type=INPUT_UINT;
    cur_inp_int_min=0;
    cur_inp_int_max=255;
    if( read_save == true ) {
      cam_repeat = cur_inp_int;
      eeprom_save(E_cam_repeat, cam_repeat);
    }
    cur_inp_int = cam_repeat;
    break;

  case 6:
    // camera repeat delay
    ui_type=INPUT_UINT;
    cur_inp_int_min=0;
    cur_inp_int_max=UINT_MAX;
    if( read_save == true) {
      delay_repeat = cur_inp_int;
      eeprom_save(E_delay_repeat, delay_repeat);
    }
    cur_inp_int = delay_repeat;
    break;

  case 7:
    // pre exp delay
    ui_type=INPUT_UINT;
    cur_inp_int_min=0;
    cur_inp_int_max=UINT_MAX;
    if( read_save == true ) { 
      delay_postexp = cur_inp_int;
      eeprom_save(E_delay_preexp, delay_preexp);
    }
    cur_inp_int = delay_preexp;
    break;

  case 8:
    // post exp delay
    ui_type=INPUT_UINT;
    cur_inp_int_min=0;
    cur_inp_int_max=UINT_MAX;
    if( read_save == true ) { 
      delay_postexp = cur_inp_int;
      eeprom_save(E_delay_postexp, delay_postexp);
    }
    cur_inp_int = delay_postexp;
    break;

  case 9:
    // focus delay / tap time
    ui_type=INPUT_UINT;
    cur_inp_int_min=0; //TODO oder 100?
    cur_inp_int_max=UINT_MAX;
    if( read_save == true ) {
      delay_focus = cur_inp_int;
      eeprom_save(E_delay_focus, delay_focus);
    }
    cur_inp_int = delay_focus;
    break;

  }
}



void get_global_set(byte pos, boolean read_save) {

  switch(pos) {

  case 0:
    // backlight level    
    ui_type=INPUT_UINT;
    cur_inp_int_min=0;
    cur_inp_int_max=255;
    if(read_save == true) {
      lcd_bkl=cur_inp_int;
      alt_io_display_set(lcd_bkl);
      eeprom_save(E_lcd_bkl, lcd_bkl);
    }
    cur_inp_int = lcd_bkl;
    break;

  case 1:
    // lcd dim time
    ui_type=INPUT_UINT;
    cur_inp_int_min=0;
    cur_inp_int_max=255;
    if( read_save == true ) {
      lcd_dim_tm = cur_inp_int;
      eeprom_save(E_lcd_dim_tm, lcd_dim_tm);
    }

    cur_inp_int = lcd_dim_tm;
    break;

  case 2:
    // blank lcd   
    ui_type=INPUT_ONOFF;
    if( read_save == true ) {
      blank_lcd = cur_inp_bool;
      eeprom_save(E_blank_lcd, blank_lcd);
    }

    cur_inp_bool = blank_lcd;
    break;

  case 3: 
    // input 1
    ui_type=INPUT_IO;
    if( read_save == true ) {
      altio_connect(0, cur_inp_int);
      eeprom_save(E_input_type0,input_type[0]);
    }
    cur_inp_int = input_type[0];
    break;

  case 4: 
    // input 2 
    ui_type=INPUT_IO;
    if( read_save == true ) {
      altio_connect(1, cur_inp_int);
      eeprom_save(E_input_type1,input_type[1]);
    }
    cur_inp_int = input_type[1];
    break;

  case 5: 
    // input delay
    ui_type=INPUT_UINT;
    cur_inp_int_min=0;
    cur_inp_int_max=UINT_MAX;
    if( read_save == true ) {
      delay_ext_in = cur_inp_int;
      eeprom_save(E_delay_ext_in, delay_ext_in);
    }
    cur_inp_int =  delay_ext_in;
    break;

  case 6:
    // output delay
    ui_type=INPUT_UINT;
    cur_inp_int_min=0;
    cur_inp_int_max=UINT_MAX;
    if( read_save == true ) {
      delay_ext_out = cur_inp_int;
      eeprom_save(E_delay_ext_out, delay_ext_out);
    }
    cur_inp_int = delay_ext_out;
    break;

  case 7:
    // USB Trigger enable
    ui_type=INPUT_ONOFF;
    if( read_save == true ) {
      if (cur_inp_bool) external_io|=EXT_INTV_USB;
      else external_io&=!EXT_INTV_USB;
      eeprom_save(E_external_io, external_io);
    }
    cur_inp_bool = ((external_io&&EXT_INTV_USB)!=0);
    break;

  case 8:
    // invert dir display
    ui_type=INPUT_ONOFF;
    if( read_save == true ) {
      ui_invdir = cur_inp_bool;
      eeprom_save(E_ui_invdir, ui_invdir);
    }
    cur_inp_bool = ui_invdir;
    break;

  case 9:
    // flip I/O trigger type
    ui_type=INPUT_ONOFF;
    if( read_save == true ) {
      altio_dir = (cur_inp_bool == false) ? FALLING : RISING;
      eeprom_save(E_altio_dir, altio_dir);
    }
    cur_inp_bool = (altio_dir == FALLING) ? false : true;
    break;

  case 10:
    // reset memory
    ui_type=INPUT_OKCANCEL;
    if( read_save == true ) {
      if( cur_inp_bool ){
        eeprom_saved(false);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(F("Reseting.."));
        motor_set_speed(0);
        delay(1000);
        restart();
       }
    }
    cur_inp_bool = false;
    break;
  }

}


void get_mainscr_set(byte pos, boolean read_save) {
  ui_float_tenths = false;


  switch(pos) {
  case 1:
    // on/off
    lcd.setCursor(0,0);
    ui_type=INPUT_ONOFF;
    if( read_save ) {
      if( cur_inp_bool > 0 ) {   
        // if set to positive value
        start_executing();
      }
      else {
        stop_executing();
      }
    }
    cur_inp_bool = S_RUNNING ; 
    break;

  case 2:
    // set interval time
    lcd.setCursor(4, 0);

    ui_type=INPUT_FLOAT;
    cur_inp_float_min=calc_total_cam_tm()/1000;
    cur_inp_float_max=UINT_MAX;
    ui_float_tenths = true;

    if( read_save ) {
      cam_interval = cur_inp_float;
      eeprom_save(E_cam_interval, cam_interval);
    }

    cur_inp_float = cam_interval;
    break;

  case 3:
    // dir for motor
    lcd.setCursor(0,1);
    ui_type=INPUT_LTRT;

    if( read_save ){
      motor_dir(cur_inp_bool);
      eeprom_save(E_m_dir,m_dir);
    }
    cur_inp_bool = m_dir;
    break;

  case 4:
    // speed for motor
    lcd.setCursor(1,1);
    ui_type=INPUT_UINT;
    cur_inp_int_min=0;
    cur_inp_int_max=255;
    if( m_mode==MODE_SMS) {
      // shoot-move-shoot?
      cur_inp_int_max = m_maxsms;
    } 
    else {       
      cur_inp_int_max=255;
    }

    if( read_save ) {
      m_speed=cur_inp_int;
      motor_set_speed( cur_inp_int); 
      eeprom_save(E_m_speed,m_speed);
    }
    
    cur_inp_int = m_speed;
    break; 
  case 5:
   //mode SMS/Cont
   lcd.setCursor(13,1);
   ui_type=INPUT_ONOFF;
  
    if( read_save ){
      m_mode=cur_inp_bool;
      eeprom_save(E_m_mode,m_mode);
    }
    cur_inp_bool = m_mode;
    break;
    
  }
}

void get_manual_select(byte pos) {

  switch (pos) {
  case 0:  
    // set in manual mode
    show_manual();
    break;

  case 1:
    //TODO
    break;
  }
}

void get_calibrate_select(byte pos) {
  // display calibrate screen  
  show_calibrate();
}

void display_spd_cpm(uint8_t spd) {
  if (m_mode==MODE_CONT){ 
  float cur_cpm = motor_calc_cpm(spd);
  lcd.print(cur_cpm, 2);
  lcd.print("cpm");
  }
  else{
    float cur_cm = motor_calc_cm(spd);
    lcd.print(cur_cm, 2);
    lcd.print("cm");
    }
}

void display_spd_pct(uint8_t spd) {
  float cur_pct =  (float) spd / (float) 255;
  cur_pct *= 100;

  if( cur_pct < 100 ) {
    lcd.print(cur_pct,1);
  }
  else {
    lcd.print((int) cur_pct, DEC);
  }
  lcd.print('%');
}


