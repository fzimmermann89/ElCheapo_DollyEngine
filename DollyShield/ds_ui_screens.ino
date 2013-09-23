/* 
 
 El Cheapo Dollyshield - UI Screens
 modified Version of Dynamic Perception LLC's DollyShield ds_ui_screens.ino
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
 UI Screen-drawing functions
 ========================================
 
 */


void prep_home_screen() {
  lcd.clear();
  lcd.setCursor(0,0);

  if(S_RUNNING) {
    // in 'external intervalometer' mode, show 'ext' instead of 'on'
    if (external_io & (EXT_INTV_1|EXT_INTV_2|EXT_INTV_USB) ) {
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
  
  //interval time
  lcd.print((float) cam_interval, 1);
  lcd.print("s ");

  //shots done
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
  
  // dir displays 
  lcd.setCursor(0,1);
  char lt = ui_invdir == true ? 'R' : 'L';
  char rt = ui_invdir == true ? 'L' : 'R';

  if( m_dir == 1 ) {
    lcd.print(lt);
  }
  else {
    lcd.print(rt);
  }


  if( ui_motor_display==CPM ) {//TODO xxx
    // display pct 
    display_spd_cpm(m_speed);
  }
  else {
    display_spd_pct(m_speed);
  }


  lcd.setCursor(13,1);
  if (m_mode==MODE_SMS){
    lcd.print("SMS");
  }
  else{
    lcd.print("Con");
  }


  // we call this here mainly to reset the
  // cursor position when in an input
  if( main_scr_input ) 
    get_mainscr_set(main_scr_input, false);
}


void main_screen_select(boolean dir) {

  byte max_inputs = 5;

  if( main_scr_input == 0) {
    //enter main scr setup
    lcd.blink();
  }

  if( dir ) {
    main_scr_input++;
  }
  else {
    main_scr_input+=max_inputs;
    DEBUG_var("+=, input=",main_scr_input);
  }

  if (main_scr_input>max_inputs) main_scr_input-=(max_inputs+1);
   DEBUG_var("if, input=",main_scr_input);
  if(main_scr_input == 0 ) {
    // exit main scr setup
    lcd.noBlink();
    return;
  }

  get_mainscr_set(main_scr_input, false);
}


void show_manual() {

  ui_ctrl_flags |= UI_MANUAL_MODE;
  DEBUG_var("uiflags",ui_ctrl_flags);
  lcd.clear();
  lcd.noBlink();

  lcd.setCursor(0, 0);


  lcd.print("[Sel] to exit");


  lcd.setCursor(0, 1);
  lcd.print("Speed: ");

  if( ui_motor_display ) {
    // display ipm 
    display_spd_cpm(m_speed);
  }
  else {
    display_spd_pct(m_speed);
  }


}


void show_calibrate() {

  // show the motor calibrate screen

  ui_ctrl_flags |= UI_CALIBRATE_MODE;

  lcd.clear();
  lcd.noBlink();

  lcd.setCursor(0,0);

  lcd.print("Cal M");
  lcd.print(" [ Slot");
  switch (m_slot){
    case 0:
      lcd.print ("A");
      break;
    case 1:
      lcd.print("B");
      break;
    default:
    lcd.print("C");
  }
  lcd.print(" ]");

}



void execute_calibrate() {

  // in calibration  
  cal_flags |= CAL_CALIBRATING;
  // floating point input
  ui_type = INPUT_FLOAT;

  ui_float_tenths = false;


  byte was_cur_pos = 0;
  byte completed = 0;   

  // sms calibration
  for( byte i = 0; i <= 1; i++ ) {
		//sms runs with 50% motor power for 5s.
		//what distance should we travel?
    float shouldtravel = (max_cpm) * (5/60) * 0.5;
    cur_inp_float = shouldtravel;
    
    //printing status
    completed++;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Running ");  
    lcd.print('[');
    lcd.print(completed, DEC);
    lcd.print(" of 8]");

    // sms moving in i dir for 5s
    motor_run_calibrate(MODE_SMS, 5, i);

    update_cal_screen();

    cal_flags&=~CAL_STEP_DONE;
    while( !(cal_flags&CAL_STEP_DONE)) {
			//wait until input is done
      ui_button_check();
    }
     //we save the ratio in array
     //example: if we traveld twice as far as supposed to, an 0.5 is saved.
      m_cal_array[m_slot][CALPOINT_SMS][i] = shouldtravel/cur_inp_float;
  }
  
  //pulse calibration
  //TODO
  for( byte i = 0; i <= 1; i++ ) {
		//run motor in slow pulse mode at 50% of min speed
		byte ths_speed=min_spd/2;
    float shouldtravel = motor_calc_cpm(ths_speed);
    cur_inp_float = shouldtravel;

    
    //printing status
    completed++;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Running ");  
    lcd.print('[');
    lcd.print(completed, DEC);
    lcd.print(" of 8]");

    // Running pulse mode
    motor_run_calibrate(MODE_CONT, ths_speed, i);

    update_cal_screen();

    cal_flags&=~CAL_STEP_DONE;
    while( !(cal_flags&CAL_STEP_DONE)) {
			//wait until input is done
      ui_button_check();
    }
     //we save the ratio in array
     //example: if we traveld twice as far as supposed to, an 0.5 is saved.
      m_cal_array[m_slot][CALPOINT_PULSE][i] = cur_inp_float;

  }
  
  // cont. calibration  
  for( byte c = 1; c <= 2; c++ ) {
    //get speed for calibration
    byte ths_spd = c == 1 ? motor_spd_cal[0] : motor_spd_cal[1];
    for( byte i = 0; i <= 1; i++ ) {
			//distance we should travel in 60s
      float shouldtravel = motor_calc_cpm(ths_spd);
      cur_inp_float = shouldtravel;

      completed++;
      Serial.print("comp:");
      Serial.println(completed);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Running ");  
      lcd.print('[');
      lcd.print(completed, DEC);
      lcd.print(" of 8]");

      // cont moving in i dir for 60s
      motor_run_calibrate(MODE_CONT, ths_spd, i);

      update_cal_screen();

      cal_flags&=~CAL_STEP_DONE;

      while(!(cal_flags&CAL_STEP_DONE)) {
				//wait until input is done
        ui_button_check();
      }
      byte point = c == 1 ? CALPOINT_LOW : CALPOINT_HIGH;
      m_cal_array[m_slot][point][i] = ( shouldtravel/cur_inp_float );
    }
  }


  cal_flags &= ~CAL_CALIBRATING;
  cal_flags |= CAL_DONE;

  // save values to memory
  // handle m_cal_array in a sane manner
  // float m_cal_array[3][4][2] 
  // 3 * 4 * 2 * 4 = 96
  //TODO
  // byte* p = (byte*)(void*)&m_cal_array;
  // eeprom_write(EEPROM_TODO, *p, (3*4*2*4));

}


void update_cal_screen() {

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Dist Moved:");
  lcd.setCursor(0,1);

  lcd.print(cur_inp_float, 2);
}



