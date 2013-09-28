/* 
 
 El Cheapo Dollyshield - IO Functions and Interrupts
 * This file needs additional effort when porting to different hardware *
 modified Version of Dynamic Perception LLC's DollyShield ds_alt_io.ino
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
 Alt I/O Handlers
 ========================================
 
 */


void altio_isr_handler(byte which) {

  // from internals
  extern volatile unsigned long timer0_millis;

  if( timer0_millis - input_trig_last > ALT_TRIG_THRESH ) {

    input_trig_last = timer0_millis;

    switch( input_type[which] ) {

    case 1:
      start_executing();
      break;

    case 2:
      stop_executing();
      break;

    case 3:
      altio_flip_runstat();
      break;

    case 4: 
      // set camera ok to fire
      external_io|= EXT_INTV_OK;
      break;

    case 8:
      // switch  motor direction
      motor_dir(!m_dir);

      break;

    default:
      break;
    } // end switch
  } //end if timer0...
}


void altio_isr_one() {
  altio_isr_handler(0);
}


void altio_isr_two() {
  altio_isr_handler(1);
}


void altio_connect(byte which, byte type) {

  input_type[which] = type;
  //disable every flag for this line
  external_io &= ~ ((EXT_TRIG_1_BEFORE|EXT_TRIG_1_AFTER|EXT_INTV_1)  << which);

// disabled
  if( type == 0 ) {
    detachInterrupt(which);
    if (which==0){
      digitalWriteFast(2, LOW);
    }
    else{
      digitalWriteFast(3, LOW);
    }
    return;
  }
  

  if( type == 5 || type == 6 || type == 7 ) {
    // output mode

    detachInterrupt(which);
    if (which==0){
      pinModeFast(2,OUTPUT);
    }
    else{
      pinModeFast(3,OUTPUT);
    }
    // set correct flag, as needed
    if( type == 5 ) {
      external_io |= (EXT_TRIG_1_BEFORE << which) ;
    }
    else if( type == 6 ) {
      external_io |= (EXT_TRIG_1_AFTER << which);
    }
    else {
      external_io |= ((EXT_TRIG_1_BEFORE|EXT_TRIG_1_AFTER) << which);
    }
  }

else {
  //input mode
  if( type == 4 ) {
    // external intervalometer function
    external_io |= EXT_INTV_1 << which;
  }

  if (which==0){
    // set pin as input
    pinModeFast(2, INPUT);
    // enable pull-up resistor
    digitalWriteFast(2, HIGH);
    attachInterrupt(0, altio_isr_one, altio_dir);
  }
  else{
  // set pin as input
    pinModeFast(3, INPUT);
    // enable pull-up resistor
    digitalWriteFast(3, HIGH);
    attachInterrupt(1, altio_isr_two, altio_dir);
  }
 }    
}
void altio_flip_runstat() {
  // if currently running, stop; if not, start

    if(S_RUNNING) {
    // running
    stop_executing();
  }
  else {
    start_executing();
  }

}

void alt_ext_trigger_engage(){
  // set the pins according to the enabled triggers
  if( external_io & (EXT_TRIG_1_AFTER|EXT_TRIG_1_BEFORE) ) 
    digitalWriteFast(2, !altio_dir);
  if( external_io & (EXT_TRIG_2_AFTER|EXT_TRIG_2_BEFORE) ) 
    digitalWriteFast(3, !altio_dir);
  // set timer to disengage trigger
  timer3_set(length_ext_out, alt_ext_trigger_disengage);
  
}

void alt_ext_trigger_disengage() {
  // set the pins according to the enabled triggers
  if( external_io & (EXT_TRIG_1_AFTER|EXT_TRIG_1_BEFORE) ) 
    digitalWriteFast(2, altio_dir);
  if( external_io & (EXT_TRIG_2_AFTER|EXT_TRIG_2_BEFORE) )
    digitalWriteFast(3, altio_dir);
}

/*
 ========================================
 Timer Functions
 ========================================
 *This is used to be independent of hardware PWMs.
 *Needed for the DFRobot Shield, which has the LCD Pins at the PWM Pins.
 *Replacement of former used Timer1 and Mstimer2 library to offer 2 timers,
 *independent from PWM functionality.
 *All the Registers are set up for ATMega 168/328 used on regular arduinos.  
 */
 
 
 

void initialize_alt_timers() {

  cli();                //disable interrupts
  //timer 2
  TCCR2A = 0x00;        //Timer2 Control Reg A: Wave Gen Mode normal
  TCCR2B = 0x04;        //set Prescaler to 64
  TIMSK2 |= (1<<OCIE2A);//enable Compare Interrupts
  TIMSK2 |= (1<<OCIE2B);  
  TIMSK2 |= (1<<TOIE2); //enable timer  
  //timer1
  TCCR1A = 0x00;        //Timer1 Control Reg A: Wave Gen Mode normal
  TCCR1B = 0x04;        //set Prescaler to 256
  TIMSK1 &= ~(1<<OCIE1A);//disable Compare Interrupts until needed
  TIMSK1 &= ~(1<<OCIE1B);  
  TIMSK1 |= (1<<TOIE1); //enable timer  



  sei();               //enable interrupts
}



void alt_io_motor_set(uint8_t value){
  if (value==0) {
    TIMSK2 &= ~(1<<OCIE2A); //disable motor-off interrupt
    digitalWriteFast(MOTOR0_P, LOW); 
  }
  else  {
    TIMSK2 |= (1<<OCIE2A); //enable motor-off interrupt
  }    
  S_SLOW_MODE=false;
  OCR2A=value;
}


void alt_io_motor_set_slow(uint8_t value){
  m_counter_max_on=m_pulse_length*value;
  m_counter_max_off=m_pulse_length*(255-value);
  m_counter_cur=m_counter_max_on;
  S_SLOW_MODE=true;
  S_SLOW_MODE_MON=true;
  OCR2A=255; //TODO anpassbar machen? kalibrierbar?
}


void alt_io_display_set(uint8_t value){
  OCR2B=value;
  if (value==0) {
    TIMSK2 &= ~(1<<OCIE2B);  //disable display-off interrupt
    digitalWriteFast(LCD_BKL,LOW); //disable backlight
  }
  else {
    TIMSK2 |= (1<<OCIE2B);     //enable display-off interrupt
  }
}

ISR(TIMER2_COMPA_vect) {
  //motor off
 digitalWriteFast(MOTOR0_P, LOW); 
}

ISR(TIMER2_COMPB_vect) {
  //display off
 digitalWriteFast(LCD_BKL, LOW);
}

ISR(TIMER2_OVF_vect){
 if (S_SLOW_MODE){
   m_counter_cur--;
   if (m_counter_cur==0){
     //time to switch
     
      if (S_SLOW_MODE_MON){
      //currently on, switch to off
      digitalWriteFast(MOTOR0_P, LOW);
      //set counter to value when motor should be turned on
      S_SLOW_MODE_MON=false;
      m_counter_cur=m_counter_max_on;
      
        }
      else{
       //currently off, switch to on
     digitalWriteFast(MOTOR0_P, HIGH);
     //set counter to value when motor should be turned off
     S_SLOW_MODE_MON=true;
     m_counter_cur=m_counter_max_off;
        }  
     }
 }
 if (S_TIMER3_SET){
  if (timer3_ms==0){
  (*timer3_func)();
  S_TIMER3_SET=false;
  }
  else timer3_ms--;
 }
  if (OCR2A>0) digitalWriteFast(MOTOR0_P, HIGH); //motor on
  if (OCR2B>0) digitalWriteFast(LCD_BKL, HIGH);  //display on

}

void timer1_set(uint16_t ms, Callback f){
  timer1_s=0;
  if (ms==0){
    //no delay given, do it instanly  
    f();
  }
  else{

    //how many times are we going to overflow? overflow happens every 1048ms
    while (ms>1048){
      timer1_s++;
      ms-=1048;
    }
    timer1_func=f;
    S_TIMER1_SET=true;
    TIMSK1 &= ~(1<<TOIE1); //disable timer while calculating compare value
    //multiply ms by 62 (should be 62.5 ticks per ms) and add it to the current timer
    //value to set new interrupt compare value
    OCR1A=TCNT1+(ms*62);
    if (OCR1A<=TCNT1) timer1_s++; //we have an overflow for the ms part if counter is greater than compare value
    if (timer1_s==0) {       //no overflow
      TIFR1  |= (1<<OCF1A);  //clear flag in case it is already set
      TIMSK1 |= (1<<OCIE1A); //enable Compare Interrupt
    }  
    TIMSK1 |= (1<<TOIE1);  //re-enable timer
  }
}
void timer2_set(uint16_t ms, Callback f){
  timer2_s=0;
  if (ms==0){
    //no delay given, do it instanly  
    f();
  }
  else{

    //how many times are we going to overflow? overflow happens every 1048ms
    while (ms>1048){
      timer2_s++;
      ms-=1048;
    }
    timer2_func=f;
    S_TIMER2_SET=true;
    TIMSK1 &= ~(1<<TOIE1); //disable timer while calculating compare value
    //multiply ms by 62 (should be 62.5 ticks per ms) and add it to the current timer
    //value to set new interrupt compare value
    OCR1B=TCNT1+(ms*62);
    if (OCR1B<=TCNT1) timer2_s++; //we have an overflow for the ms part if counter is greater than compare value
    if (timer2_s==0) {       //no overflow
      TIFR1  |= (1<<OCF1B);  //clear flag in case it is already set
      TIMSK1 |= (1<<OCIE1B); //enable Compare Interrupt
    }
    TIMSK1 |= (1<<TOIE1);  //re-enable timer
  }
}  

void timer3_set(uint16_t ms, Callback f){
   if (ms==0){
    //no delay given, do it instanly  
    f();
  }
  else{
    timer3_ms=ms;
    timer3_func=f;
    S_TIMER3_SET=true;
  }
}
ISR(TIMER1_COMPA_vect) {
  //timer 1
  TIMSK1 &= ~(1<<OCIE1A);
  (*timer1_func)();
  S_TIMER1_SET=false;
}

ISR(TIMER1_COMPB_vect) {
  //timer 2
  TIMSK1 &= ~(1<<OCIE1B);
  (*timer2_func)();
  S_TIMER2_SET=false;
   
}

ISR(TIMER1_OVF_vect){
  if (S_TIMER1_SET){
    timer1_s--;
    if (timer1_s==0) {
      TIFR1  |= (1<<OCF1A); //clear flag if compare match happend while interrupt disabled
      TIMSK1 |= (1<<OCIE1A);//enable compare interrupt
    }
  }
  if (S_TIMER2_SET){
    timer2_s--;
    if (timer2_s==0) {
      TIFR1  |= (1<<OCF1B); //clear flag if compare match happend while interrupt disabled
      TIMSK1 |= (1<<OCIE1B);//enable compare interrupt
    }
  }
}
void clear_delay(){
  S_IN_DELAY=false;
}

void restart(){
  asm volatile ("  jmp 0");  
}
