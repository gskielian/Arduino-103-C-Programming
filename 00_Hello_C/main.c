/*
    5-10-07
    Copyright Spark Fun Electronics� 2007
    Nathan Seidle
    nathan at sparkfun.com
    
    ATmega168
	
	Example Blink
	Toggles all IO pins at 1Hz
*/

#include <avr/delay.h>
#include <avr/io.h>

//Define functions
//======================
void io_init(void);      // Initializes IO
void pwm_init(void);     // Initializes PWM
//======================

int main (void) {
  io_init(); //Setup IO pins and defaults
  pwm_init();

  while(1) {
    for (int i = 0x00; i < 0x7f; ++i) {
      OCR0A = i; // these are probably pins 5 and 61`o
      _delay_ms(1000);
    }
  }

  return 0;
}

void io_init (void) {
  //1 = output, 0 = input
  DDRD = _BV(PD5) | _BV(PD6);
}

void pwm_init(void) {
  TCCR0A = _BV(COM0A0) | _BV(COM0B0) | _BV(WGM01);
  TCCR0B = _BV(CS00);
  OCR0A = 0x00;
  OCR0B = 0x00;
}
