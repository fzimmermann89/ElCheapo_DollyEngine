/*

 El Cheapo Dollyshield - Motor control functions
 modified Version of Dynamic Perception LLC's DollyShield
 (c) 2010-2011 C.A. Church / Dynamic Perception LLC ds_motor.ino
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
 Motor control functions
 ========================================
 
 */


void motor_speed_adjust(int val, boolean spd_floor ) {

  byte c_speed = 0;
  // val is expected to be between -255 and 255;

  if( (int) m_speed+ val >= 255 ) {
    c_speed = 255;
  }
  else {
    c_speed = (int) m_speed + val >= 0 ? m_speed + val : 0;
    // do we need to floor the value at the min speed setting? (man control)
    if( spd_floor )
      c_speed = c_speed < min_spd ? min_spd : c_speed;
  }

  motor_set_speed(c_speed );

}

void motor_control(boolean state) {

  // turn motor on or off

  if( ! state ) {
    //turn off
    motor_set_speed( 0 );
    m_cur_speed = 0;
    S_MOT_RUNNING=false;
  }
  else {
    // set motors as running...
    S_MOT_RUNNING=true;
    if( m_cur_speed > 0 ) motor_set_speed(m_cur_speed);
  }

}

void motor_set_speed( uint8_t speed ) { 
//TODO: Calibration
  if (speed==0){
   //called to disable motor.
    S_MOT_RUNNING=false;
    m_sms_tm=0; //disables if in sms-mode //TODO Should disable timer
    m_cur_speed=0; //disables if in cont.-mode
    alt_io_motor_set(0); //disables slow mode too.
  }
  else if (!(ui_ctrl_flags&UI_MANUAL_MODE)){ 
    //not in manual mode
    m_cur_speed=speed;
    if (m_mode==MODE_SMS){
      //SMS mode
      //calculate times
      ;
    }
    else if (speed<min_spd){
      //speed below min_speed, so use pulsing mode
      if (S_MOT_RUNNING) alt_io_motor_set_slow(m_cur_speed); //write only to motor if running!
    }
    else{
      //continous mode
      if (S_MOT_RUNNING) alt_io_motor_set(m_cur_speed); //write only to motor if running!
    }
  }
  else{
    //manual mode //TODO
    alt_io_motor_set(m_cur_speed);
  }
  return;
}

void motor_dir(byte dir ) {
//TODO: SMS
  if( m_dir == dir ) //already running in the right direction
    return;

  // get current speed for the motor and save it for writing back later
  byte ths_speed = m_cur_speed;


  if( ths_speed > 0 ) {
    // motor was already moving, need to stop
    // and let motor settle before moving
    // stop motor
    motor_set_speed( 0 );
    delay(100);
  }


  digitalWriteFast(MOTOR0_DIR, dir);
  m_dir = dir;
  //writing back saved speed
  motor_set_speed( ths_speed );
}


float motor_calc_cpm(uint8_t spd) {
/*
 // calculate ipm for a given speed and mode

    // get max speed for either pulse or sms mode
  // on calibration screen, always ch

  float maxspd = ( ! ths_mode ) ? (float) m_maxsms : 255.0;

  // in manual mode, we're always in 0-255 mode
  if( ui_ctrl_flags & B00000100 )
    maxspd = 255.0;

  float cur_cpm = (float) max_cpm * ( (float) spd / (float) maxspd );

  return(cur_cpm);
  */
return (0.0);
}

float motor_calc_cm(uint8_t spd)
{
}


void motor_update_dist(float rpm, float diarev ) {
  
  // set distance settings when rpm or diarev change
   
   max_cpm = rpm * diarev;
   min_spd = 255 * ( min_cpm / max_cpm );
   m_maxsms= max_cpm * 100; //TODO
   
   eeprom_save(E_max_cpm, max_cpm);
   eeprom_save(E_min_spd, min_spd);
   eeprom_save(E_m_maxsms, m_maxsms);
   
}
uint16_t motor_calc_sms_tm(uint8_t speed)
{
    return speed*motor_cal_adjust(MODE_SMS,speed,m_dir);
}


void motor_sms_run() {
  timer1_set(m_sms_tm,motor_sms_stop);
  digitalWriteFast(MOTOR0_P,HIGH);
  
}

void motor_sms_stop() {
  digitalWriteFast(MOTOR0_P,LOW);   
  motor_ran++;
}

uint8_t motor_calc_speed_ramp_lead(uint8_t cur_speed, uint16_t cur_shots) {
  //to be called after a shoot to calculate speed for next one

  if (cur_shots<=m_lead_in){
    //during m_lead_in
    return 0;
  }
  else if (cur_shots<=(m_lead_in+m_ramp_in)){
    //during m_ramp_in
    //calculate speed
    uint8_t speed_step;
    uint8_t remaining_shots_for_ramp=(m_lead_in+m_ramp_in)-cur_shots+1;
    speed_step=(m_speed-cur_speed)/(remaining_shots_for_ramp+1);
    return (cur_speed+speed_step);
  }  
  else if (cam_max>0){
    //ramp and lead out only make sense if max. shots are set.
    if (cur_shots<(cam_max-m_lead_out-m_ramp_out)){
      //during normal mode
      return m_speed;
    }
    else if (cur_shots<(cam_max-m_lead_out)){
      //during m_ramp_out
      //calculate speed
      uint8_t speed_step;
      uint8_t remaining_shots_for_ramp=cam_max-m_lead_out-cur_shots;
      speed_step=(cur_speed)/(remaining_shots_for_ramp+1);
      return (cur_speed-speed_step);
    } 
    else{
        //during m_lead_out or finished.
        return 0;
    }
  }  
  else {
    //no cam_max set. so normal mode.
    return m_speed;
  }
}
   
float motor_cal_adjust(byte type, byte speed, byte dir) {
   
   //TODO: über calibration array nachdenken.
   
 
  if( type == MODE_SMS )
    //sms mode
    return(m_cal_array[m_angle][CALPOINT_SMS][dir]);
  if (speed<min_spd) {
    //pulse
    return (m_cal_array[m_angle][CALPOINT_PULSE][dir]);
    }
  else{
    //cont mode
    // determine which calibration position we fall
    // into
   
    byte pos = 0;
   
    // if between two cal points, get position between them
    byte cal_diff = motor_spd_cal[CALPOINT_HIGH] - motor_spd_cal[CALPOINT_LOW];
    byte hi_diff  = 255 - motor_spd_cal[1];
   
    if ( speed> motor_spd_cal[0] && speed < motor_spd_cal[1] ) {
      //between
      //calculate percentage of "low" and "high" to use
      unsigned int diff = speed - motor_spd_cal[0];
      float diff_pct = (float) diff / (float) cal_diff;
    
      float ret = ( m_cal_array[m_angle][CALPOINT_HIGH][dir] * diff_pct ) + ( m_cal_array[m_angle][CALPOINT_LOW][dir] * ( 1.0 - diff_pct ) );
      return(ret);
    }
    else if( speed> motor_spd_cal[1] ) {
      // between high cal point and max speed
      unsigned int diff = speed - motor_spd_cal[1];
      float diff_pct = (float) diff / (float) hi_diff;
      // float ret = m_cal_array[0][m_angle[0]][2][dir] - (m_cal_array[0][m_angle[0]][2][dir] * diff_pct); //TODO
      float ret = m_cal_array[m_angle][CALPOINT_HIGH][dir] - (m_cal_array[m_angle][CALPOINT_HIGH][dir] * diff_pct);
      return(ret);
    }
    else if( speed <= motor_spd_cal[0] ) {
      //below or at low point
      return(m_cal_array[m_angle][CALPOINT_LOW][dir]);
    }
    else if( speed == motor_spd_cal[1] ) {
      //at high point
      return(m_cal_array[m_angle][CALPOINT_HIGH][dir]);
   }
   else {
     //where are we now? //TODO
    return(1.0);
   }
  }
}

void motor_run_calibrate(byte which, unsigned int mspd, byte dir) {
  /* Serial.print("w:");
   Serial.println(which);
   Serial.print("msp:");
   Serial.println(mspd);
   
   byte cur_dir = m_dir[0];
   motor_dir(dir);
   
   if( which == 1 ) {
   Serial.print("maxs:");
   Serial.println(m_maxsms[0]);
   float m_pct = ( (float) mspd / (float) m_maxsms[0] );
   unsigned int run_tm = 60000.0 * m_pct;
   Serial.print("pct:");
   Serial.println(m_pct);
   
   motor_ran = 0;
   
   run_motor_sms();
   MsTimer2::set(run_tm, stop_motor_sms);
   MsTimer2::start();
   
   while( ! motor_ran )
   continue;
   
   return;
   }
   else {
   Serial.println("pulsing");
   byte was_on_pct = on_pct[0];
   byte was_off_pct = off_pct[0];
   
   motor_calc_pulse_len(mspd, true);
   
   unsigned long run_tm = millis();
   Serial.println("start");
   motor_run_pulsing();
   
   // main loop is ~ 10% slower than calibration loop
   while( millis() - run_tm < 58000 ) {
   // introduce timing block delay similar to main loop
   int foo = analogRead(BUT_PIN);
   }
   Serial.println("stop");
   motor_stop_all();
   
   on_pct[0]  = was_on_pct;
   off_pct[0] = was_off_pct;
   
   }
   
   motor_dir(cur_dir);
   */
}


uint8_t calc_speed(float cpm){
  return 128;
}

float calc_cpm(uint8_t speed){
  return 1.0;
}





