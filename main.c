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
/* tests the max frequency 0x00 which is for 8mHz
  while(1) {
    OCR0A = 0; // these are probably pins 5 and 6
    _delay_ms(1000);
  }
*/

//OCR0A Stands for:
  while(1) {
    for (int i = 0xff; i > 0x00; --i) { // 0x7f is approximately half way
      OCR0A = i; // OCR0A is actually for pin 5 (B is for pin 6)
      _delay_ms(1000);
    }
  }

  return 0;
}

void io_init (void) {
  //1 = output, 0 = input
  DDRD = _BV(PD5) | _BV(PD6); //_BV is a macro function
}

void pwm_init(void) {
  //TCCR0A = _BV(COM0A0) | _BV(COM0B0) | _BV(WGM01); // this version maintains 50% duty cycle but changes the period
  TCCR0A = _BV(COM0A1) | _BV(WGM00); // this version does pwm with period of 32 us
  TCCR0B = _BV(CS00);
  TCNT0 = 0x00;
  OCR0A = 0x00;
  OCR0B = 0x00;
}
