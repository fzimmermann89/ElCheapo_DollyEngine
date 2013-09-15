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
  ui_type = INPUT_ONOF;

  switch(menu) {
  case 1:
    get_manual_select(pos);
    break;
  case 2:    
    get_m_axis_set(pos, read_save);
    break;
  case 3:    
    get_m_cam_set(pos, read_save);
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
      // increase value
      cur_inp_float += mod;
    }
    else {
      if( cur_inp_float < mod ) {
        cur_inp_float = 0.0;
      }
      else {
        cur_inp_float -= mod;
      }

    }

  }
  else if( ui_type==INPUT_ONOF ||ui_type==INPUT_LTRT ||ui_type==INPUT_CMPCT || ui_type==INPUT_CONTSMS ) {
    // any boolean type

    cur_inp_bool = ! cur_inp_bool;
  }
  else {
    // unsigned long type
    unsigned long mod = (1 * inp_val_mult);
    // long input
    if( dir == true ) {
      cur_inp_long += mod;
    }
    else {
      if( cur_inp_long < mod ) {
        cur_inp_long = 0;
      }
      else {
        cur_inp_long -= mod;
      }
    } // end if dir not true

    // ceiling on certain values
    if (ui_type==INPUT_ANGLE ) {
      cur_inp_long = cur_inp_long > 2 ? 2 : cur_inp_long;
    }
    else if (ui_type==INPUT_IO ) {
      cur_inp_long = cur_inp_long > 8 ? 8 : cur_inp_long;
    }
    else if (ui_type==INPUT_SHUTTER ) {
      cur_inp_long = cur_inp_long > 3 ? 3 : cur_inp_long;
    }

  } // end else long type...

}




void get_m_axis_set( byte pos, boolean read_save) {

  ui_type = INPUT_LONG;

  // set axis configurable values

  switch(pos) { 
  case 0:
    ui_type=INPUT_CONTSMS;
    // set Movement Mode (SMS/CONT)
    if( read_save == true ) {
      m_mode = cur_inp_bool;
      eeprom_write(EEPROM_TODO, m_mode);
    }

    cur_inp_bool = m_mode;
    break;

  case 1:
    // set ramp in value
    if( read_save == true ) {
      m_ramp_in=cur_inp_long;         
      eeprom_write(EEPROM_TODO, m_ramp_in);
    }

    cur_inp_long = m_ramp_in;
    break;

  case 2:
    // set ramp out value
    if( read_save == true ) {
      m_ramp_out=cur_inp_long;         
      eeprom_write(EEPROM_TODO, m_ramp_out);
    }

    cur_inp_long = m_ramp_out;
    break;

  case 3: 
    // doly angle (for calibration)
    ui_type=INPUT_ANGLE;

    if( read_save == true ) {
      m_angle = cur_inp_long;
      eeprom_write(EEPROM_TODO, m_angle);
    }

    cur_inp_long = m_angle;
    break;

  case 4:
    // set lead-in value
    if( read_save == true ) {
      m_lead_in = cur_inp_long;
      eeprom_write(EEPROM_TODO, m_lead_in);
    }

    cur_inp_long = m_lead_in;
    break;

  case 5:
    // set lead-out value
    if( read_save == true ) {
      m_lead_out = cur_inp_long;
      eeprom_write(EEPROM_TODO, m_lead_out);
    }

    cur_inp_long = m_lead_out;
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
    ui_type =INPUT_FLOAT;
    // set rpm
    if( read_save == true ) {
      m_rpm = cur_inp_float;
      motor_update_dist(m_rpm, m_diarev);
      eeprom_write(EEPROM_TODO, m_rpm);
    }

    cur_inp_float = m_rpm;
    break;

  case 2:
    // distance per revolution
    ui_type=INPUT_FLOAT;

    if( read_save == true ) {
      m_diarev = cur_inp_float;
      motor_update_dist(m_rpm, m_diarev);
      eeprom_write(EEPROM_TODO, m_diarev);
    }
    cur_inp_float = m_diarev;
    break;

  case 3:
    // min cont setting //TODO: Input in CPM instead of speed
    ui_type=INPUT_FLOAT;
    if( read_save == true ) {
      min_cpm = cur_inp_float;
      min_spd = 255 * ( min_cpm / max_cpm );

      eeprom_write(EEPROM_TODO, min_cpm);
      eeprom_write(EEPROM_TODO, min_spd);
    } 
    cur_inp_float = min_cpm;
    break;

    //TODO 4+5

  case 6:
    // low calibration spd
    ui_type=INPUT_LONG; //ui_type_flags2 |= B01000000;
    if( read_save == true ) {
      motor_spd_cal[0] = cur_inp_long;
      eeprom_write(EEPROM_TODO, motor_spd_cal[0]);
    }
    cur_inp_long = motor_spd_cal[0];
    break;

  case 7:
    // high calibration spd
    ui_type=INPUT_LONG; //ui_type_flags2 |= B01000000;
    if( read_save == true ) {
      motor_spd_cal[1] = cur_inp_long;
      eeprom_write(EEPROM_TODO, motor_spd_cal[1]);
    }
    cur_inp_long = motor_spd_cal[1];
    break;
  }

}


void get_m_cam_set( byte pos, boolean read_save ) {

  // reset this flag
  ui_float_tenths = false;


  switch(pos) {
  case 0:
    // interval timer
    ui_type=INPUT_FLOAT;
    ui_float_tenths = true;

    if( read_save == true ) { 
      cam_interval = cur_inp_float;
      eeprom_write(EEPROM_TODO, cam_interval);
    }
    cur_inp_float = cam_interval;
    break;

  case 1:
    // max shots
    ui_type=INPUT_LONG;
    if( read_save == true ) {
      cam_max = cur_inp_long;
      eeprom_write(EEPROM_TODO, cam_max);
    }
    cur_inp_long = cam_max;
    break;

  case 2:
    // exposure time
    ui_type=INPUT_LONG;
    if( read_save == true ) { 
      exp_tm = cur_inp_long;
      eeprom_write(EEPROM_TODO, exp_tm);
    }
    cur_inp_long = exp_tm;
    break;    

  case 3:
    //Shutter Type
    ui_type=INPUT_SHUTTER;
    if( read_save == true ) { 
      shutter_mode = cur_inp_long;
      eeprom_write(EEPROM_TODO, shutter_mode);
    }
    cur_inp_long = shutter_mode;
    break;

  case 4:
    //bulb mode
    ui_type=INPUT_ONOF;

    if( read_save == true ) {
      bulb_mode = cur_inp_bool;
      eeprom_write(EEPROM_TODO, bulb_mode);
    }

    cur_inp_bool = bulb_mode;
    break;

  case 5:
    // camera repeat value
    ui_type=INPUT_LONG;
    if( read_save == true ) {
      cur_inp_long = cur_inp_long > 255 ? 255 : cur_inp_long;
      cam_repeat = cur_inp_long;
      eeprom_write(EEPROM_TODO, cam_repeat);
    }
    cur_inp_long = cam_repeat;
    break;

  case 6:
    // camera repeat delay
    ui_type=INPUT_LONG;
    if( read_save == true) {
      cam_rpt_dly = cur_inp_long;
      eeprom_write(EEPROM_TODO, cam_rpt_dly);
    }
    cur_inp_long = cam_rpt_dly;
    break;

  case 7:
    // pre exp delay
    ui_type=INPUT_LONG;
    if( read_save == true ) { 
      post_delay_tm = cur_inp_long;
      eeprom_write(EEPROM_TODO, pre_delay_tm);
    }
    cur_inp_long = pre_delay_tm;
    break;

  case 8:
    // post exp delay
    ui_type=INPUT_LONG;
    if( read_save == true ) { 
      post_delay_tm = cur_inp_long;
      eeprom_write(EEPROM_TODO, post_delay_tm);
    }
    cur_inp_long = post_delay_tm;
    break;

  case 9:
    // focus delay / tap time
    ui_type=INPUT_LONG;
    if( read_save == true ) {
      focus_tap_tm = cur_inp_long;
      eeprom_write(EEPROM_TODO, focus_tap_tm);
    }
    cur_inp_long = focus_tap_tm;
    break;

  }
}



void get_global_set(byte pos, boolean read_save) {

  switch(pos) {

  case 0:
    // backlight level    
    ui_type=INPUT_LONG;
    if(read_save == true) {
      cur_bkl = cur_inp_long > 255 ? 255 : cur_inp_long;
      ui_set_backlight(cur_bkl);
      eeprom_write(EEPROM_TODO, cur_bkl);
    }
    cur_inp_long = cur_bkl;
    break;

  case 1:
    // lcd dim time
    ui_type=INPUT_LONG;
    if( read_save == true ) {
      lcd_dim_tm = cur_inp_long;
      eeprom_write(EEPROM_TODO, lcd_dim_tm);
    }

    cur_inp_long = lcd_dim_tm;
    break;

  case 2:
    // blank lcd   
    ui_type=INPUT_ONOF;
    if( read_save == true ) {
      blank_lcd = cur_inp_bool;
      eeprom_write(EEPROM_TODO, blank_lcd);
    }

    cur_inp_bool = blank_lcd;
    break;

  case 3: 
    // input 1
    ui_type=INPUT_IO;
    if( read_save == true ) {
      altio_connect(0, cur_inp_long);
      eeprom_write(EEPROM_TODO,input_type[0]);
    }
    cur_inp_long = input_type[0];
    break;

  case 4: 
    // input 2 
    ui_type=INPUT_IO;
    if( read_save == true ) {
      altio_connect(1, cur_inp_long);
      eeprom_write(EEPROM_TODO,input_type[1]);
    }
    cur_inp_long = input_type[1];
    break;

  case 5: 
    // input delay
    ui_type=INPUT_LONG;
    if( read_save == true ) {
      ext_in_delay = cur_inp_long;
      eeprom_write(EEPROM_TODO, ext_in_delay);
    }
    cur_inp_long =  ext_in_delay;
    break;

  case 6:
    // output delay
    ui_type=INPUT_LONG;
    if( read_save == true ) {
      ext_out_delay = cur_inp_long;
      eeprom_write(EEPROM_TODO, ext_out_delay);
    }

    cur_inp_long = ext_out_delay;
    break;

  case 7:
    // GB enable
    ui_type=INPUT_ONOF;
    if( read_save == true ) {
      gb_enabled = cur_inp_bool;
      eeprom_write(EEPROM_TODO, gb_enabled);
    }

    cur_inp_bool = gb_enabled;
    break;

  case 8:
    // invert dir display
    ui_type=INPUT_ONOF;
    if( read_save == true ) {
      ui_invdir = cur_inp_bool;
      eeprom_write(EEPROM_TODO, ui_invdir);
    }

    cur_inp_bool = ui_invdir;
    break;

  case 9:
    // flip I/O trigger type
    ui_type=INPUT_ONOF;
    if( read_save == true ) {
      altio_dir = (cur_inp_bool == false) ? FALLING : RISING;
      eeprom_write(EEPROM_TODO, altio_dir);
    }

    cur_inp_bool = (altio_dir == FALLING) ? false : true;
    break;

  case 10:
    // reset memory
    ui_type=INPUT_ONOF;

    if( read_save == true ) {
      if( cur_inp_bool )
        eeprom_saved(false);
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
    ui_type=INPUT_ONOF;
    if( read_save ) {
      if( cur_inp_bool > 0 ) {   
        // if set to positive value
        start_executing();
      }
      else {
        stop_executing();
      }
    }
    cur_inp_bool =S_RUNNING ; //run_status >> 7
    break;

  case 2:
    // set interval time
    lcd.setCursor(4, 0);

    ui_type=INPUT_FLOAT;
    ui_float_tenths = true;

    if( read_save ) {
      cam_interval = cur_inp_float;
      eeprom_write(EEPROM_TODO, cam_interval);
    }

    cur_inp_float = cam_interval;
    break;

  case 3:
    // dir for motor
    lcd.setCursor(0,1);
    ui_type=INPUT_LTRT;

    if( read_save )
      motor_dir(cur_inp_bool);


    cur_inp_bool = m_dir;
    break;

  case 4:
    // speed for motor
    lcd.setCursor(1,1);
    ui_type=INPUT_LONG;
    if( m_mode==MODE_SMS) {
      // shoot-move-shoot?
      cur_inp_long = cur_inp_long > m_maxsms ? m_maxsms: cur_inp_long;
    } 
    else {       
      cur_inp_long = cur_inp_long > 255 ? 255 : cur_inp_long;
    }

    if( read_save ) {
      motor_set_speed((unsigned int) cur_inp_long); 
      // calculate speed change per shot for ramping
      // if needed - use function to update values
      //  motor_set_ramp(m_ramp_set);  //TODO
    }
    cur_inp_long = m_speed;
    break; 
  }
}



void get_manual_select(byte pos) {

  switch (pos) {
  case 0:  
    // set in manual mode
    ui_ctrl_flags |= B00000100;
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

void display_spd_cpm(unsigned int spd) {

  float cur_ipm = motor_calc_cpm(spd, m_mode);
  lcd.print(cur_ipm, 2);
  lcd.print("cm/min");

}

void display_spd_pct(byte spd) {
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


