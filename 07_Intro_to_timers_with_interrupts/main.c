//timers
//
//
//this is where the code meets the metal

//what we need



// system clock frequency
// //
  // 16mHZ
  // 8 mHz
// timer resolution:
  // choices
  //
  //

// 328p has 3 times,
//
// 2x 8 bit timers and 
// 1x 16 bit timer
//
//
// 8bit timer counts and compares to a value you give it
// when they equal it does something (97 of manual)
//
// create a time which will generate a timer
//
//
//
// LED for practice (can't afford 20 oscopes -- or can we?)
//
// each student gets a multimeter, advanced class, each student gets an oscope,
//
//

// 8 bit timer
// TCCR0A -- Timer/Counter Control Register A
// will let us set up CTC mode -- when they match it jumps to interrupt && clear timer


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

int extraTime;
int main(void) {

  DDRB =   (1 << PORTB0); //sets pin 8 to be an output

  //CTC Mode
  TCCR0A = (1 << WGM01); //WGM01 sets this for CTC mode

  //OCR0A  Output Compare Register A
  OCR0A = 195;  //number of ticks we want to go through before resetting, this is the compare, when = it does stuff + resets

//TCCR0B = (1 << CS01) | (1 << CS00); //page 110 of the documentaion  this sets the prescaler to 1024

  //TIMSK0 register controls the interrupts-- when this happens I want this interrupt to occu
  //OCIE0A this bit want interrupt setup so that when compare 'A' is equal it will set up an interrupt
  TIMSK0 = (1 << OCIE0A);

  // clock select bit prescaler

  int i;
  sei(); // sets the i-bit
  while(1) {
  //OCR0A  Output Compare Register A
  //

/*
  TCCR0B =  (0 << CS02) | (1 << CS01) | (0 << CS00); //page 110 of the documentaion  this sets the prescaler to 1024

  for (i = 255; i > 0 ; i--) {
    OCR0A = i;  //number of ticks we want to go through before resetting, this is the compare, when = it does stuff + resets
    _delay_ms(100);
  }
  */

  /*
  for (i = 0; i < 255 ; i++) {
    OCR0A = i;  //number of ticks we want to go through before resetting, this is the compare, when = it does stuff + resets
    _delay_ms(100);
  }
  */

//----------------------------------------------------------
//                        ROUND 2
//----------------------------------------------------------
  TCCR0B =  (1 << CS02) | (0 << CS01) | (0 << CS00); //page 110 of the documentaion  this sets the prescaler to 1024
    OCR0A = 255;  //number of ticks we want to go through before resetting, this is the compare, when = it does stuff + resets
    _delay_ms(5000);
    OCR0A = 1;  //number of ticks we want to go through before resetting, this is the compare, when = it does stuff + resets
    _delay_ms(5000);
  TCCR0B =  (0 << CS02) | (1 << CS01) | (1 << CS00); //page 110 of the documentaion  this sets the prescaler to 1024
    OCR0A = 255;  //number of ticks we want to go through before resetting, this is the compare, when = it does stuff + resets
    _delay_ms(5000);
    OCR0A = 1;  //number of ticks we want to go through before resetting, this is the compare, when = it does stuff + resets
    _delay_ms(5000);
    /*
  for (i = 255; i > 35; i--) {
    OCR0A = i;  //number of ticks we want to go through before resetting, this is the compare, when = it does stuff + resets
    _delay_ms(100);
  }
  */

  TCCR0B =  (0 << CS02) | (0 << CS01) | (1 << CS00); //page 110 of the documentaion  this sets the prescaler to 1024
  for (i = 255; i > 35; i--) {
    OCR0A = i;  //number of ticks we want to go through before resetting, this is the compare, when = it does stuff + resets
    _delay_ms(10);
  }

  }
}


ISR(TIMER0_COMPA_vect) {
    PORTB ^=  (1 << PORTB0); //toggles an led -- this is really useful!!
}
