/* 
 
 "DollyShield" MX2
 
 (c) 2010 C.A. Church / Dynamic Perception LLC
 
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
  else if( ui_type_flags & B01111110 ) {
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

    if( INPUT_ANGEL ) {
      // ceiling on certain values
      cur_inp_long = cur_inp_long > 2 ? 2 : cur_inp_long;
    }
    else if( INPUT_IO ) {
      // ceiling for alt i/o types
      cur_inp_long = cur_inp_long > 8 ? 8 : cur_inp_long;
    }

  } // end else long type...

}




void get_m_axis_set( byte pos, boolean read_save) {

  ui_type_flags = 0;
  ui_type_flags2 = 0;

  // set axis configurable values

  switch(pos) { 
  case 0:
    // set ramp value
    if( read_save == true ) {
      motor_set_ramp(cur_inp_long);         
      eeprom_write(61, m_ramp_set[0]);
    }

    cur_inp_long = m_ramp_set[0];
    break;

  case 1:
    // set lead-in value
    if( read_save == true ) {
      m_lead_in[0] = cur_inp_long;
      eeprom_write(229 , m_lead_in[0]);
    }

    cur_inp_long = m_lead_in[0];
    break;

  case 2:
    // set lead-out value
    if( read_save == true ) {
      m_lead_out[0] = cur_inp_long;
      eeprom_write(233, m_lead_out[0]);
    }

    cur_inp_long = m_lead_out[0];
    break;

  case 3:
    ui_type_flags |= B10000000;
    // set rpm
    if( read_save == true ) {
      m_rpm[0] = cur_inp_float;
      motor_update_dist(m_rpm[0], m_diarev[0]);
      eeprom_write(32, m_rpm[0]);
    }

    cur_inp_float = m_rpm[0];
    break;


  case 4: 

    // doly angle (for calibration)
    ui_type_flags |= B00000001;

    if( read_save == true ) {
      m_angle[0] = cur_inp_long;
      eeprom_write(215, m_angle[0]);
    }

    cur_inp_long = m_angle[0];
    break;


  case 5:
    // calibrate motor
    get_calibrate_select(0);
    break;

  case 6:
    // calibration constant

    ui_type_flags |= B10000000;

    if( read_save == true ) {
      m_cal_constant[0] = cur_inp_float;
      eeprom_write(239, m_cal_constant[0]);
    }

    cur_inp_float = m_cal_constant[0];
    break;

  case 7:
    // min ipm setting
    ui_type_flags |= B10000000;
    if( read_save == true ) {

      min_ipm[0] = cur_inp_float;
      min_spd[0] = 255 * ( min_ipm[0] / max_ipm[0] );
      
      
       Serial.print("mins:");
  Serial.println(min_spd[0]);
      
      eeprom_write(40, min_ipm[0]);
      eeprom_write(48 , min_spd[0]);
    } 
    cur_inp_float = min_ipm[0];

    break;

  case 8:
    // distance per revolution
    ui_type_flags |= B10000000;

    if( read_save == true ) {
      m_diarev[0] = cur_inp_float;
      motor_update_dist(m_rpm[0], m_diarev[0]);
      eeprom_write(16, m_diarev[0]);
    }

    cur_inp_float = m_diarev[0];

    break;

  case 9:
    // motor min pulse
    if( read_save == true ) {

      if(cur_inp_long > 255)
        cur_inp_long = 255;

      m_min_pulse[0] = cur_inp_long;          
      eeprom_write(50, m_min_pulse[0]);
    }
    cur_inp_long = m_min_pulse[0];
    break;


  }

}




void get_m_cam_set( byte pos, boolean read_save ) {

  // reset this flag
  ui_float_tenths = false;


  switch(pos) {
  case 0:
    // interval timer
    ui_type_flags |= B10000000;
    ui_float_tenths = true;

    if( read_save == true ) { 
      cam_interval = cur_inp_float;
      eeprom_write(67, cam_interval);
    }
    cur_inp_float = cam_interval;
    break;

  case 1:
    // max shots
    if( read_save == true ) {
      cam_max = cur_inp_long;
      eeprom_write(10, cam_max);
    }
    cur_inp_long = cam_max;
    break;

  case 2:
    // exposure time
    if( read_save == true ) { 
      exp_tm = cur_inp_long;
      eeprom_write(260, exp_tm);
    }
    cur_inp_long = exp_tm;
    break;    

  case 3:
    // post exp delay
    if( read_save == true ) { 
      post_delay_tm = cur_inp_long;
      eeprom_write(5, post_delay_tm);
    }
    cur_inp_long = post_delay_tm;
    break;

  case 4:
    // focus tap tm
    if( read_save == true ) {
      focus_tap_tm = cur_inp_long;
      eeprom_write(3, focus_tap_tm);
    }
    cur_inp_long = focus_tap_tm;
    break;

  case 5:
    //ir remote
    ui_type_flags |= B01000000;
    if( read_save == true ) {
      ir_remote= cur_inp_bool;
      eeprom_write(267, ir_remote); //TODO
    }
    cur_inp_bool = ir_remote;
    break;

  case 6:
    // focus w/ shutter
    ui_type_flags |= B01000000;
    if( read_save == true ) {
      focus_shutter = cur_inp_bool;
      eeprom_write(7, focus_shutter);
    }
    cur_inp_bool = focus_shutter;
    break;

  case 7:
    // camera repeat value
    if( read_save == true ) {
      cur_inp_long = cur_inp_long > 255 ? 255 : cur_inp_long;
      cam_repeat = cur_inp_long;
      eeprom_write(249, cam_repeat);
    }
    cur_inp_long = cam_repeat;
    break;

  case 8:
    // camera repeat delay
    if( read_save == true) {
      cam_rpt_dly = cur_inp_long;
      eeprom_write(250, cam_rpt_dly);
    }
    cur_inp_long = cam_rpt_dly;
    break;
  }
}



void get_global_set(byte pos, boolean read_save) {

  ui_type_flags  = 0;
  ui_type_flags2 = 0;

  switch(pos) {

  case 0:
    // motor display type
    ui_type_flags |= B00001000;      

    if( read_save == true ) {
      ui_motor_display = cur_inp_bool;
      eeprom_write(56, ui_motor_display);
    }

    cur_inp_bool = ui_motor_display;
    break;

  case 1:
    // motor slow type
    ui_type_flags |= B00000100;      

    if( read_save == true ) {
      motor_sl_mod = cur_inp_bool;
      eeprom_write(57, motor_sl_mod);
    }        

    cur_inp_bool = motor_sl_mod;
    break;

  case 2:

    // backlight level    
    if(read_save == true) {
      cur_bkl = cur_inp_long > 255 ? 255 : cur_inp_long;
      ui_set_backlight(cur_bkl);
      eeprom_write(266, cur_bkl);
    }

    cur_inp_long = cur_bkl;
    break;

  case 3:
    // lcd dim time
    if( read_save == true ) {
      lcd_dim_tm = cur_inp_long;
      eeprom_write(58, lcd_dim_tm);
    }

    cur_inp_long = lcd_dim_tm;
    break;

  case 4:
    // blank lcd   
    ui_type_flags |= B01000000;

    if( read_save == true ) {
      blank_lcd = cur_inp_bool;
      eeprom_write(60, blank_lcd);
    }

    cur_inp_bool = blank_lcd;
    break;

  case 5: 
    // input 1
    ui_type_flags2 |= B10000000;

    if( read_save == true ) {
      altio_connect(0, cur_inp_long);
      eeprom_write(217,input_type[0]);
    }

    cur_inp_long = input_type[0];
    break;

  case 6: 
    // input 2 
    ui_type_flags2 |= B10000000;

    if( read_save == true ) {
      altio_connect(1, cur_inp_long);
      eeprom_write(218,input_type[1]);
    }

    cur_inp_long = input_type[1];
    break;

  case 7:
    // metric display
    ui_type_flags |= B01000000;

    if( read_save == true ) {
      if ( cur_inp_bool != ui_is_metric ) {
        // only convert values when the 
        // UI metric type changes

        if( ui_is_metric ) {
          // going to imperial
          m_diarev[0] = m_diarev[0] / 2.54;
          min_ipm[0]  = min_ipm[0] / 2.54;
          m_diarev[1] = m_diarev[1] / 2.54;
          min_ipm[1]  = min_ipm[1] / 2.54;
        }
        else {
          // going to metric
          m_diarev[0] *= 2.54;
          min_ipm[0]  *= 2.54;
          m_diarev[1] *= 2.54;
          min_ipm[1]  *= 2.54;
        }
        ui_is_metric = cur_inp_bool;

        // write values to memory
        eeprom_write(219, ui_is_metric);
        eeprom_write(16, m_diarev[0]);
        eeprom_write(20, m_diarev[1]);  
        eeprom_write(40, min_ipm[0]);
        eeprom_write(44, min_ipm[1]);

        motor_update_dist(m_rpm[0], m_diarev[0]);
      }
    }
    cur_inp_bool = ui_is_metric;
    break;

  case 8:
    // reset memory
    ui_type_flags |= B01000000;

    if( read_save == true ) {
      if( cur_inp_bool )
        eeprom_saved(false);
    }

    cur_inp_bool = false;
    break;

  case 9:
    // low calibration spd
    ui_type_flags2 |= B01000000;

    if( read_save == true ) {
      motor_spd_cal[0] = cur_inp_long;
      eeprom_write(237, motor_spd_cal[0]);
    }

    cur_inp_long = motor_spd_cal[0];
    break;

  case 10:
    // high calibration spd
    ui_type_flags2 |= B01000000;

    if( read_save == true ) {
      motor_spd_cal[1] = cur_inp_long;
      eeprom_write(238, motor_spd_cal[1]);
    }

    cur_inp_long = motor_spd_cal[1];
    break;

  case 11: 
    // alt output pre time

    if( read_save == true ) {
      ext_trig_pre_delay = cur_inp_long;
      eeprom_write(252, ext_trig_pre_delay);
    }

    cur_inp_long = ext_trig_pre_delay;
    break;

  case 12:
    // alt output post time

    if( read_save == true ) {
      ext_trig_pst_delay = cur_inp_long;
      eeprom_write(256, ext_trig_pst_delay);
    }

    cur_inp_long = ext_trig_pst_delay;
    break;

  case 13:
    // GB enable
    ui_type_flags |= B01000000;

    if( read_save == true ) {
      gb_enabled = cur_inp_bool;
      eeprom_write(264, gb_enabled);
    }

    cur_inp_bool = gb_enabled;
    break;

  case 14:
    // invert dir display
    ui_type_flags |= B01000000;

    if( read_save == true ) {
      ui_invdir = cur_inp_bool;
      eeprom_write(265, ui_invdir);
    }

    cur_inp_bool = ui_invdir;
    break;

  case 15:
    // flip I/O trigger type

    ui_type_flags |= B01000000;

    if( read_save == true ) {
      altio_dir = (cur_inp_bool == false) ? FALLING : RISING;
      eeprom_write(52, altio_dir);
    }

    cur_inp_bool = (altio_dir == FALLING) ? false : true;
    break;

  }

}


void get_mainscr_set(byte pos, boolean read_save) {

  // clear out previous on/off select

  ui_type_flags   = 0;
  ui_type_flags2  = 0;
  ui_float_tenths = false;



  switch(pos) {
  case 1:
    // on/off
    lcd.setCursor(0,0);

    if( read_save ) {
      if( cur_inp_bool > 0 ) {   
        // if set to positive value
        start_executing();
      }
      else {
        stop_executing();
      }
    }

    ui_type_flags |= B01000000;

    cur_inp_bool =S_RUNNING ; //run_status >> 7
    break;

  case 2:
    // set interval time
    lcd.setCursor(4, 0);

    ui_type_flags |= B10000000;
    ui_float_tenths = true;

    if( read_save ) {
      cam_interval = cur_inp_float;
      eeprom_write(67, cam_interval);
    }

    cur_inp_float = cam_interval;
    break;

  case 3:
    // dir for m1
    lcd.setCursor(0,1);

    if( read_save )
      motor_dir(cur_inp_bool);

    ui_type_flags |= B00010000;
    cur_inp_bool = m_wasdir[0];
    break;

  case 4:
    // spd for m1
    lcd.setCursor(1,1);

    if( ! motor_sl_mod ) {
      // shoot-move-shoot?
      cur_inp_long = cur_inp_long > m_maxsms[0] ? m_maxsms[0] : cur_inp_long;
    } 
    else {       
      cur_inp_long = cur_inp_long > 255 ? 255 : cur_inp_long;
    }

    if( read_save ) {
      motor_set_speed((unsigned int) cur_inp_long); 
      // calculate speed change per shot for ramping
      // if needed - use function to update values
      motor_set_ramp(m_ramp_set[0]);
    }

    cur_inp_long = m_speed;

    break; 
  }
}



void get_manual_select(byte pos) {


  // set in manual mode
  ui_ctrl_flags |= B00000100;

  if( pos == 1 ) {
    //TODO
    return;
  }
  // show manual motor screen
  show_manual();
}


void get_calibrate_select(byte pos) {
  // display calibrate screen  
  show_calibrate();
}

void display_spd_ipm(unsigned int spd) {

  float cur_ipm = motor_calc_ipm(spd, motor_sl_mod);
  lcd.print(cur_ipm, 2);

  // handle metric conversion
  if( ui_is_metric ) {
    lcd.print('c');
  }
  else {
    lcd.print('i');
  }
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

