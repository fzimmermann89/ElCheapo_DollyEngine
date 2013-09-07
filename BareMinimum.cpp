#line 1 "BareMinimum.ino"
#include <avr/io.h>
#include <avr/interrupt.h>

#include "Arduino.h"
void setup();
void loop();
void test();
void writemotor(uint8_t value);
void writedisplay(uint8_t value);
#line 4
uint16_t volatile timeoutms;

void (*timeoutfunc)();

void settimeout(uint16_t ms,void (*f)()){
  timeoutms=ms;
  timeoutfunc=f;
}

void setup() {
  Serial.begin(115200);
  // PWM Port einstellen

  DDRD = 0xFF;         // Port als Ausgang
  // zus\u00e4tzliche PWM-Ports hier setzen
  
  // Timer 2 vorbereiten
  cli();
  TCCR2A = 0x00;        //Timer2 Control Reg A: Wave Gen Mode normal
  TCCR2B = 4;             // Timer l\u00e4uft mit Prescaler 64
  TIMSK2 |= (1<<OCIE2A);   // Interrupts freischalten
  TIMSK2 |= (1<<OCIE2B);  
  TIMSK2 |= (1<<TOIE2);   
                   // Interrupts global einschalten

writedisplay(128);
writemotor(128);
sei(); 
settimeout(200,test);
PORTD|=_BV(4);
}

void loop() {
  if (Serial.available()) {
   int mode = Serial.parseInt();
    Serial.print("prescaler:");
    Serial.println(mode);
    TCCR2B=mode;
    int value = Serial.parseInt();
    Serial.print("value:");
    Serial.println(value);
    writemotor(value);
  }
}
void test(){
  Serial.println("test");
  delay(50);
  timeoutms=200;
}



void writemotor(uint8_t value){
  OCR2A=value;
}

void writedisplay(uint8_t value){
  OCR2B=value;
}

ISR(TIMER2_COMPA_vect) {
  //motor off
  PORTD &= ~_BV(2);   
}

ISR(TIMER2_COMPB_vect) {
  //display off
  PORTD &= ~_BV(3);
}

ISR(TIMER2_OVF_vect){
    
  if (timeoutms==0){
     PORTD&= ~_BV(4);
  (*timeoutfunc)();
  }
  else timeoutms--;
  if (OCR2A>0) PORTD|=_BV(2); //motor on
  if (OCR2B>0)PORTD|=_BV(3); //display on

}


