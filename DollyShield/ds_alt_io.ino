/* 
 
 MX2 El Cheapo - IO Functions and Interrupts
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
      motor_dir(!m_wasdir);

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

    if(S_RUNNING) { // run_status & B10000000
    // running
    stop_executing();
  }
  else {
    start_executing();
  }

}

void alt_ext_trigger_engage(unsigned long length) {
  // set flag
  S_EXT_TRIG_ENGAGED=true;

  // we use the interrupt pins, 2&3

    if( external_io & (EXT_TRIG_1_AFTER|EXT_TRIG_1_BEFORE) ) 
      digitalWriteFast(2, !altio_dir);
    if( external_io & (EXT_TRIG_2_AFTER|EXT_TRIG_2_BEFORE) ) 
      digitalWriteFast(3, !altio_dir);
 
  //MsTimer2::set(length, alt_ext_trigger_disengage); //TODO
  //MsTimer2::start();
}

void alt_ext_trigger_disengage() {

    if( external_io & (EXT_TRIG_1_AFTER|EXT_TRIG_1_BEFORE) ) 
      digitalWriteFast(2, altio_dir);
    if( external_io & (EXT_TRIG_2_AFTER|EXT_TRIG_2_BEFORE) )
      digitalWriteFast(3, altio_dir);
 
  //MsTimer2::stop();   //TODO

  // clear flag...
  S_EXT_TRIG_ENGAGED=false;//run_status &= B11110111;
}

/*

 ========================================
 Timer Functions
 ========================================
 
 */
 
void settimeout(uint16_t ms,void (*f)()){
  timer2_ms=ms;
  timer2_func=f;
}

void initialize_alt_timers() {

  cli();                //disable interrupts
  //timer 2
  TCCR2A = 0x00;        //Timer2 Control Reg A: Wave Gen Mode normal
  TCCR2B = 4;           //set Prescaler to 64
  TIMSK2 |= (1<<OCIE2A);//enable Compare Interrupts
  TIMSK2 |= (1<<OCIE2B);  
  TIMSK2 |= (1<<TOIE2);   
  
  //timer1



  sei();               //enable interrupts
}



void alt_io_motor_set(uint8_t value){
  OCR2A=value;
}

void alt_io_display_set(uint8_t value){
  OCR2B=value;
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
    
  if (timer2_ms==0){
     PORTD&= ~_BV(4);
  (*timer2_func)();
  }
  else timer2_ms--;
  if (OCR2A>0) digitalWriteFast(MOTOR0_P, HIGH); //motor on
  if (OCR2B>0) digitalWriteFast(LCD_BKL, HIGH);  //display on

}





