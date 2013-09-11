/* 
 
 MX2 El Cheapo - Motor control functions
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
    Serial.println("fl");
      c_speed = c_speed < min_spd ? min_spd : c_speed;
  }

  motor_set_speed(c_speed );   

}

void motor_control(boolean state) {

  // turn motors on or off

  if( ! state ) {
    // set motors as not running

    unsigned int ths_spd = m_speed;

    motor_set_speed( 0 );

    m_speed = ths_spd;
    mcur_spds = ths_spd;

   S_MOT_RUNNING=false;// run_status &= B11101111;

  }
  else {
    // set motors as running...
   S_MOT_RUNNING=true;// run_status |= B00010000;
    if( mcur_spds > 0 )
      motor_set_speed(mcur_spds);
  }
}



void motor_set_speed( unsigned int m_speed ) {
analogWrite(MOTOR0_P,32);
delay(500);

  Serial.print("s:");
Serial.println(m_speed);

  m_speed = m_speed;
  m_sms_tm[0] = 0;



  if( ! motor_sl_mod && ! (ui_ctrl_flags & B00000100) ) {
    // handle when in interleaved mode and not on
    // manual control screen

    float m_pct = ( (float) m_speed / (float) m_maxsms[0] );  

    m_sms_tm[0] = 60000.0 * m_pct;

    // calibrate
    m_sms_tm[0] *= motor_cal_adjust(0,0,m_wasdir[0]);

  }
  else {

    // normalize to max pwm speed
    m_speed = m_speed > 255 ? 255 : m_speed;
Serial.print("s_norm:");
Serial.println(m_speed);

    m_speed = m_speed;
  }


  // do we need to go into pulsing mode?

  if( m_speed > 0 && m_speed < min_spd[0]  ) {
Serial.println("smaller");
    motor_calc_pulse_len(m_speed, false);

  } //      
  else {
    on_pct[0] = 0;
  }




  if( ! (S_MOT_RUNNING)  ) { //run_status & B00010000
    // if disabled, do not move motor, but
    // instead adjust stored speed
    mcur_spds[0] = m_speed;
    return;
  }
  else if( ! (ui_ctrl_flags & B00000100) && m_sms_tm[0] > 0 ) {
    // just in case
    digitalWrite(MOTOR0_P, LOW);
    Serial.println("sms-man");
    // return if we're in an SMS condition
    // and not in manual mode
    return;
  }

  // if we've made it this far, set motor pin
  // to given analog speed

  // only set analog speed if it exceeds min speed
Serial.print("ms:");
Serial.println(min_spd[0]);
   Serial.print("s:");
    Serial.println(m_speed);
    
  if( m_speed >= min_spd[0] ) {
    analogWrite(MOTOR0_P, m_speed);
 Serial.print("written");
 Serial.println(m_speed);
  }
  else {
    Serial.println("off");
    // just in case... switching down from 
    // pwm to pulsed...
    digitalWrite(MOTOR0_P,LOW);
  }

}

void motor_calc_pulse_len(unsigned int m_speed, boolean ignore_cal) {
  // for how many periods should the motor be on and
  // off? (pulsing mode)     
Serial.println("calc");
  float m_pct   = ( (float) m_speed / 255.0 );
  float periods = 1333.0; // 25 times a second 

  on_pct[0] = periods * m_pct; 
  off_pct[0] = (float) (periods - on_pct[0]);

  float cal_amt = motor_cal_adjust(1,m_speed, m_wasdir[0]);

  // calibrate, if desired
  if( ! ignore_cal && cal_amt != 1.0 )
    off_pct[0] = ( (double) off_pct[0] * ( cal_amt * m_cal_constant[0] )  ); 

  if(on_pct[0] < 1)
    on_pct[0] = 1;

  // make sure that we're on for a minimum amount of time

  if ( on_pct[0] != 0 && on_pct[0] < m_min_pulse[0] ) {
    // adjust so that off time is increased relative to on time
    float diff = (float) m_min_pulse[0] / (float) on_pct[0];       
    off_pct[0] = ((float) off_pct[0] * diff);
    on_pct[0] = m_min_pulse[0];
  }

}

void motor_dir(byte dir ) {

  if( m_wasdir[0] == dir )
    return;

  // get current speed for the motor
  byte ths_speed = m_speed;



  if( ths_speed > 0 ) {
    // motor was already moving, need to stop
    // and let motor settle before moving
    // stop motor
    motor_set_speed( 0 );
    delay(100);    
  }


  digitalWriteFast(MOTOR0_DIR, dir);  
  m_wasdir[0] = dir;
  motor_set_speed( ths_speed );
}


float motor_calc_ipm(unsigned int spd, boolean ths_mode) {
  // calculate ipm for a given speed and mode

    // get max speed for either pulse or sms mode
  // on calibration screen, always ch

  float maxspd = ( ! ths_mode ) ? (float) m_maxsms[0] : 255.0;

  // in manual mode, we're always in 0-255 mode
  if( ui_ctrl_flags & B00000100 ) 
    maxspd = 255.0;

  float cur_ipm = (float) max_ipm[0] * ( (float) spd / (float) maxspd );      

  return(cur_ipm);
}


void motor_update_dist(float rpm, float diarev ) {
  // set distance settings when rpm or diarev change

  max_ipm[0] = rpm * diarev;
  min_spd[0] = 255 * ( min_cpm[0] / max_ipm[0] );
  Serial.print("mins:");
  Serial.println(min_spd[0]);
  m_maxsms[0] = max_ipm[0] * 100;

  eeprom_write(EEPROM_TODO, max_ipm[0]);
  eeprom_write(EEPROM_TODO, min_spd[0]);
  eeprom_write(EEPROM_TODO, m_maxsms[0]);

}

void motor_pulse() { //TODO

  // this function is called by timer1 to pulse motors
  // on and off in pulsing mode

    if( ! timer_engaged )
    return;

  volatile static byte mstate[2] = {
    0,0  };
  volatile static unsigned long pulses[2]  = {
    1,1  };

  volatile static byte pos = 0;

  pos++;


  for( byte i = 0; i < MAX_MOTORS; i++ ) {
    if( on_pct[i] > 0 ) {
      // speed is below min cont. speed

      if( ! mstate[i] ) {
        if( pulses[i] < off_pct[i] ) {
          pulses[i]++;
          continue;
        }
        else {            
          // set port value high for given motor
        //  PORTD |= (B00100000 << i); 
         digitalWriteFast(MOTOR0_P,HIGH); 
          mstate[i] = 1;
          pulses[i] = 1;
        }
      }
      else {
        if( pulses[i] < on_pct[i] ) {
          pulses[i]++;
          continue;
        }
        else {

          // set port value low for given motor
       //   PORTD &= ( B11111111 ^ ( B00100000 << i ) ); 
         digitalWriteFast(MOTOR0_P,LOW); 
          mstate[i] = 0;
          pulses[i] = 1;
        }
      } // end else not mstate...
    }  // end if on_pct...
  }  // end for

}



void run_motor_sms() {

  analogWrite(MOTOR0_P, 255);

} 

void stop_motor_sms() {

  MsTimer2::stop();

  analogWrite(MOTOR0_P, 0);

  motor_ran++;

} 

void motor_set_ramp( byte ramp) {
  // set motor ramp value, adjust
  // associated values




  m_ramp_set[0]   = ramp > 255 ? 255 : ramp; //TODO: immer [0]

  // calculate speed change per shot  
  if( ramp > 0 ) {
    m_ramp_shift[0] = (float) m_speed / ramp;

    // if there's less than one step per jump,
    // we need to skip shots between increases
    // so determine how many shots to skip

      if( m_ramp_shift[0] < 1 ) {
      m_ramp_mod[0] = ramp / m_speed;
      m_ramp_mod[0] = m_ramp_mod[0] < 2 ? 2 : m_ramp_mod[0];
      m_ramp_shift[0] = 1.0;
    }
    else {
      m_ramp_mod[0] = 0;
    }

  }
  else {
    m_ramp_shift[0] = 0;
    m_ramp_mod[0]   = 0;
  }


}  


void motor_stop_all() {
  // stop all motors

  // disable pulsing interrupt if engaged
  if( timer_engaged ) {
    Timer1.detachInterrupt();
    timer_engaged = false;
  }

  digitalWrite(MOTOR0_P, LOW);

  motor_control(false);



}


float motor_cal_adjust(byte type, byte m_spd, byte dir) {



  // simplistic for sms mode
  if( type == 0 ) 
    return(m_cal_array[0][m_angle[0]][0][dir]);

  // determine which calibration position we fall
  // into

  byte pos = 0;

  // if between two cal points, get position between them
  byte cal_diff = motor_spd_cal[1] - motor_spd_cal[0];
  byte hi_diff  = 255 - motor_spd_cal[1];

  if ( m_spd > motor_spd_cal[0] && m_spd < motor_spd_cal[1] ) {
    unsigned int diff = m_spd - motor_spd_cal[0];
    float diff_pct = (float) diff / (float) cal_diff;
    float ret = ( m_cal_array[0][m_angle[0]][2][dir] * diff_pct ) + ( m_cal_array[0][m_angle[0]][1][dir] * ( 1.0 - diff_pct ) );
    return(ret);
  }
  else if( m_spd > motor_spd_cal[1] ) {
    // between last cal point and max speed
    unsigned int diff = m_spd - motor_spd_cal[1];
    float diff_pct = (float) diff / (float) hi_diff;
    float ret = m_cal_array[0][m_angle[0]][2][dir] - (m_cal_array[0][m_angle[0]][2][dir] * diff_pct);
    return(ret);
  }
  else if( m_spd <= motor_spd_cal[0] ) {
    pos = 1;
  }
  else if( m_spd == motor_spd_cal[1] ) {
    pos = 2;
  }
  else {
    return(1.0);
  }

  return(m_cal_array[0][m_angle[0]][pos][dir]);
}


void motor_run_pulsing() {

  // start pulsing motor movement

  if( ! timer_engaged ) {

    // we use timer1, which disables pwm on
    // lcd bkl pin
    if( cur_bkl > 0 ) {
      digitalWriteFast(LCD_BKL, HIGH);
    }
    else {
      digitalWriteFast(LCD_BKL, LOW);
    }

    digitalWrite(MOTOR0_P, LOW);


    Timer1.initialize(MP_PERIOD);
    Timer1.attachInterrupt(motor_pulse);
    timer_engaged = true;
    timer_used = true;

  }
}

void motor_execute_ramp_changes() {

  // check for ramping, and ramp up or down as needed

  // no ramp, go to next motor
  if( m_ramp_set[0] == 0 )
    return;

  // handle lead-in
  if( shots <= m_lead_in[0] ) {
    motor_set_speed(0); 
    return;  
  }

  // ramp up?
  if( m_ramp_set[0] >= ( shots - m_lead_in[0]) ) {
    // if ramping less than once per shot 
    if( m_ramp_mod[0] > 0 && ( shots - m_lead_in[0] ) % m_ramp_mod[0] == 0 ) {
      motor_set_speed(m_speed + 1);
    }
    else if( m_ramp_mod[0] == 0 ) {  
      motor_set_speed((m_ramp_shift[0] * (shots - m_lead_in[0]) ) );
    }
  }
  else if( (cam_max - shots - m_lead_out[0]) <= m_ramp_set[0] ) {
    // ramping down, it seems
    if( m_ramp_mod[0] > 0 && (cam_max - shots - m_lead_out[0]) % m_ramp_mod[0] == 0 ) {
      byte m_spd = m_speed > 0 ? m_speed - 1 : 0;
      motor_set_speed(m_spd);
    }
    else if( m_ramp_mod[0] == 0 ) {  
      motor_set_speed(m_ramp_shift[0] * (cam_max - shots - m_lead_out[0]) );
    }
  }


}


void motor_run_calibrate(byte which, unsigned int mspd, byte dir) {
Serial.print("w:");
Serial.println(which);
    Serial.print("msp:");
Serial.println(mspd);

  byte cur_dir = m_wasdir[0];
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

}







