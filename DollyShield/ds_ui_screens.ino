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
 UI Screen-drawing functions
 ========================================
 
 */


void prep_home_screen() {
  lcd.clear();
  lcd.setCursor(0,0);

  if(S_RUNNING) {//run_status & B10000000
    // in 'external intervalometer' mode, show 'ext' inseatd of 'on'
    if( external_interval & B11000000 || gb_enabled == true ) {
      lcd.print("Ext");
    }
    else {
      lcd.print("On");
    }
  }
  else {
    lcd.print("Off");
  }

  lcd.setCursor(4, 0);
}


void show_home() {

  prep_home_screen();  

  // deal with interval times that are less than total time
  // required between shots
  float i_total = calc_total_cam_tm();

  if( cam_interval < i_total ) {
    lcd.print(i_total, 1);
  }
  else {
    lcd.print((float) cam_interval, 1);
  }

  lcd.print("s ");


  if( shots > 999 ) {
    lcd.setCursor(10,0);
  }
  else if( shots > 99 ) {
    lcd.setCursor(11, 0);
  }
  else if( shots > 9 ) {
    lcd.setCursor(12, 0);
  }
  else {
    lcd.setCursor(13,0);
  }

  lcd.print('[');
  lcd.print(shots, DEC);
  lcd.print(']');

  lcd.setCursor(0,1);

  // dir displays
  char lt = ui_invdir == true ? 'R' : 'L';
  char rt = ui_invdir == true ? 'L' : 'R';

  if( m_wasdir[0] == 1 ) {
    lcd.print(lt);
  }
  else {
    lcd.print(rt);
  }


  if( ui_motor_display ) {
    // display pct 
    display_spd_ipm(m_speed);
  }
  else {
    display_spd_pct(m_speed);
  }


  lcd.setCursor(8,1);



  // we call this here mainly to reset the
  // cursor position when in an input
  if( main_scr_input ) 
    get_mainscr_set(main_scr_input, false);
}


void main_screen_select(boolean dir) {

  byte max_inputs = 4;
 
  if( main_scr_input == 0) {
   //enter main scr setup
    lcd.blink();
  }

  if( dir ) {
    main_scr_input++;
  }
  else {
    main_scr_input+=max_inputs;
  }

  if (wert>max_inputs) wert-=max_inputs;

  if(main_scr_input == 0 ) {
    // exit main scr setup
    lcd.noBlink();
    return;
  }

  get_mainscr_set(main_scr_input, false);
}


void show_manual() {

  ui_ctrl_flags |= B00000100;

  lcd.clear();
  lcd.noBlink();

  lcd.setCursor(0, 0);


  lcd.print("[Sel] to exit");


  lcd.setCursor(0, 1);
  lcd.print("Speed: ");

  if( ui_motor_display ) {
    // display ipm 
    display_spd_ipm(m_speed);
  }
  else {
    display_spd_pct(m_speed);
  }


}


void show_calibrate() {

  // show the motor calibrate screen

  ui_ctrl_flags |= B00000001;

  lcd.clear();
  lcd.noBlink();

  lcd.setCursor(0,0);

  lcd.print("Cal M");
  lcd.print(" [");

  byte angle = m_cur_cal * 45;

  lcd.print(angle, DEC);
  lcd.print(" Deg]");

}



void execute_calibrate() {

  // in calibration  
  ui_cal_scrn_flags |= B10000000;
  // floating point input
  ui_type = INPUT_FLOAT;

  ui_float_tenths = false;


  byte was_cur_pos = 0;
  byte completed = 0;   

  // sms calibration
  for( byte i = 0; i <= 1; i++ ) {
    float traveled = 0.01 * (max_ipm[0]);
    unsigned int runspd = 0.01 * m_maxsms[0];
    cur_inp_float = traveled;
    completed++;
Serial.println("s");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Running ");  
    lcd.print('[');
    lcd.print(completed, DEC);
    lcd.print(" of 6]");

    // sms moving in i dir
    // at 6% of total distance
    motor_run_calibrate(1, runspd, i);

    update_cal_screen();

    m_cal_done = false;

    while( m_cal_done == false ) {
      byte held = ui_button_check();
    }

    m_cal_array[0][m_cur_cal][0][i] = traveled / cur_inp_float;

  }


  // pulse calibration  
  for( byte c = 1; c <= 2; c++ ) {
    byte ths_spd = c == 1 ? motor_spd_cal[0] : motor_spd_cal[1];
Serial.print("c:");
Serial.println(c);
    for( byte i = 0; i <= 1; i++ ) {
      Serial.print("i:");
Serial.println(i);

      float des_ipm = motor_calc_ipm(ths_spd, true);
      cur_inp_float = des_ipm;

      completed++;
Serial.print("comp:");
Serial.println(completed);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Running ");  
      lcd.print('[');
      lcd.print(completed, DEC);
      lcd.print(" of 6]");

      // pulse moving in i dir
      motor_run_calibrate(2, ths_spd, i);

      update_cal_screen();

      m_cal_done = false;

      while(  m_cal_done == false ) {
        byte held = ui_button_check();
      }

      m_cal_array[0][m_cur_cal][c][i] = ( cur_inp_float / des_ipm );
    }
  }


  ui_cal_scrn_flags &= B01111111;
  ui_cal_scrn_flags |= B01000000;

  // save values to memory
  // handle m_cal_array in a sane manner
  // float m_cal_array[2][3][3][2] 
  // 2 * 3 * 3 * 2 * 4 = 144

  byte* p = (byte*)(void*)&m_cal_array;
  eeprom_write(71, *p, 144);

}


void update_cal_screen() {

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Dist Moved:");
  lcd.setCursor(0,1);

  lcd.print(cur_inp_float, 2);
}

