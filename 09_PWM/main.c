#include <avr/delay.h>
#include <avr/io.h>
#include <stdint.h>



//Define functions
//======================
void io_init(void);      // Initializes IO
void pwm_init(void);     // Initializes PWM
//======================

int main (void) {
  io_init(); //Setup IO pins and defaults
  pwm_init();

//  while(1) {
//    for (int i = 0x00; i < 0x7f; ++i) {
//      OCR0A = i; // these are probably pins 5 and 61`o
//      _delay_ms(1000);
//    }
 // }
  uint8_t i;

  for (i = 255; i > 0 ; i--) {
    OCR1AL = i;
    _delay_ms(50);
  }
  OCR1AL = 0;
  return 0;
}

void io_init (void) {
  //1 = output, 0 = input
  DDRB = _BV(PB1) | _BV(PB2);
}

void pwm_init(void) {
  TCCR1A = _BV(COM1B0); // COM1B0 indicates toggling OCR1B (which is arduino pin 10) on Compare Match aka PWM.
//  (0x81) TCCR1B ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10 136
//  (0x80) TCCR1A COM1A1 COM1A0 COM1B1 COM1B0 – – WGM11 WGM10 134
//
  OCR1AH = 0x00; //this controls the higher half of the 16 bit "TOP" value


  TCCR1B = _BV(WGM02) | _BV(CS10); // the CS stuff sets the 
  /*
//  (0x81) TCCR1B ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10 136
//    (0x80) TCCR1A COM1A1 COM1A0 COM1B1 COM1B0 – – WGM11 WGM10 134

     Clock Select Bit Description
     CS12 CS11 CS10 
     Description
     0 0 0 No clock source (Timer/Counter stopped).
     0 0 1 clkI/O/1 (No prescaling)
     0 1 0 clkI/O/8 (From prescaler)
     0 1 1 clkI/O/64 (From prescaler)
     1 0 0 clkI/O/256 (From prescaler)
     1 0 1 clkI/O/1024 (From prescaler)
     1 1 0 External clock source on T1 pin. Clock on falling edge.
     1 1 1 External clock source on T1 pin. Clock on rising edg
     */
  //there are three waveform-generation-mode bits: WGM03 WGM02 WGM01 WGM00 -- mode 3 (0100) is CTC mode (clear timer on compare match)
  //in order to create the 
//  OCR0A = 0x00; // this sets the initial pwm value in hex
//  OCR0B = 0x00; // this sets the initial pwm value in hex
}
