/*
 * GccApplication2.c
 *
 * Created: 13.09.2013 00:34:01
 *  Author: thunder
 */ 

#define F_CPU 16000000L
#include <avr/io.h>
#include <avr/interrupt.h>
#include "digitalWriteFast.h"
#include "Arduino.h"
#include "FastIO.h"
#include <util/atomic.h>

//IR Settings
#define FREQ 38400
#define oscd 16 //TODO
#define IR_PIN 5
//IR sequences

	//NIKON
unsigned int seq_nikon[]={14,0,1,2,3,4,5,6,7,8,9,10,11,12,13};
    //CANON
unsigned int seq_canon[]={3,1,2,3};

unsigned int *seqs[] = {seq_nikon,seq_canon};

	volatile uint8_t  debug8;
	volatile uint16_t debug16;
	
void send_ir(uint8_t brand);
void setup();
void loop();
void test();
void writemotor(uint8_t value);
void writedisplay(uint8_t value);
uint16_t volatile timeoutms;

void (*timeoutfunc)();

void settimeout(uint16_t ms,void (*f)()){
	timeoutms=ms;
	timeoutfunc=f;
}

void send_ir(uint8_t brand){
  for(uint8_t i=1;i<=*(seqs[brand]);i++){
    uint8_t ir_status=0;
    int  n=*(seqs[brand] + i);
    while(n>0){
      n--;
      delayMicroseconds(oscd);
	  ir_status=!ir_status;
  			digitalWriteFast(IR_PIN, (ir_status&&(i%2))); 
	   
	}
  }
}
int main(){
	digitalWrite(1,HIGH);
	digitalWriteFast(1,LOW);
	WRITE(1,HIGH);
	send_ir(0);
	send_ir(1);
	volatile uint8_t a;
	volatile uint8_t b;
	b=6;
	a=HIGH;
	digitalWrite(1,a);
	digitalWriteFast(1,a);
	WRITE(1,a);
	digitalWrite(b,a);
	digitalWriteFast(b,a);
//	WRITE(b,a);

	setup();
	while (1)
	{
		loop();
	}
	return 0;
}
void setup() {
	
	
	cli();
	
	digitalToggleFast(13);
	
	
	volatile uint8_t ergebnis;
	volatile uint8_t zahl1=255;
	volatile uint8_t zahl2=13;
	ergebnis=zahl1/zahl2;
	
	sei();
	
	
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
pinModeFast(13,OUTPUT);
pinMode(4,OUTPUT);
digitalWrite(14,HIGH);
	writedisplay(128);
	writemotor(128);
	
	settimeout(200,test);
	PORTD|=_BV(4);
	 uint8_t a=2;
	 volatile uint8_t b=2;
	 volatile uint8_t c=30;
	 	 volatile uint8_t d;
	c=c>>a;
	c=c>>b;
	c|=a;
	c|=b;
	if (d ==  b&&c) d=42;
	if (d == (uint8_t) ((uint8_t)b && (uint8_t)c )) d=37;

	
	sei();
}

void loop() {
	PORTD|=_BV(4);
	PORTD&=~_BV(4);
	digitalWriteFast(3,HIGH);
}

void test(){
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


