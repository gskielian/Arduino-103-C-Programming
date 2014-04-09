#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>

//this is for the uart code
#include <stdio.h>


//this library simplifies the return data
#include <string.h>

//set the baud rate
#define F_CPU 16000000UL
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1)

//define TX RX buffer sizes
#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 128

//these are some macros which help set hte bits
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))


//PUL  Portb2
//DIR  Portb3
//ENA  Portb4

#define PUL PB2
#define DIR PB3
#define ENA PB4
#define LED PB5

#define OPTO PD2


//Uart setup
void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);

//set up streams
FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input  = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
//end uart setup


//unsigned long
unsigned long duration; //automatically initialized to be zero
int count_dots;
int response_number;

//Define functions
//======================
void io_init(void);      // Initializes IO
void pwm_init(void);     // Initializes PWM
void set_baud(void);
void shake_sequence(uint8_t number);
//void find_first_well(void); // TODO find the pwm speed for Servo.h 


//demo sequences
//void demo_centrifuge_stage(void); // TODO add control method for customization
//void demo_full_sequence(void); // TODO align sequence in a function
//======================

int main (void) {
  io_init(); //Setup IO pins and defaults
  set_baud();

  char input;

  while (1) {

    input = getchar();

    //first stage will be a simple switch case`


    switch (input) {
      case '1':
        //shake sequence
        PORTB |= _BV(ENA); // ON the enable bit
        shake_sequence(2); // TODO shake twice -- can mod this later for custom shakes
        PORTB &= ~_BV(ENA);// OFF the enable bit
        putchar('1');
        break;
      case '2':
        //ramp sequence
        PORTB |= _BV(ENA); // ON the enable bit
        //demo_centrifuge_stage(); // TODO add control method for customization
        PORTB &= ~_BV(ENA);// OFF the enable bit
        putchar('2');
        break;
      case '3':
        //recording_sequence
        PORTB |= _BV(ENA); // ON the enable bit
        //find_first_well(); // TODO find the pwm speed for Servo.h 
        //and emulate to avoid needing to perform trial and error
        PORTB &= ~_BV(ENA);// OFF the enable bit
        putchar('3');
        break;
      case '4':
        //full_sequence
        PORTB |= _BV(ENA); // ON the enable bit
        //demo_full_sequence();// TODO create full sequence
        PORTB &= ~_BV(ENA);// OFF the enable bit
        putchar('4');
        break;
      case '5':
        //LED ON
        PORTB |= _BV(LED); // TODO Test LED ON
        putchar('5');
        break;
      case '6':
        //LED OFF
        PORTB &= ~_BV(LED); // TODO Test LED OFF
        putchar('6');
        break;
      default:
        putchar('0');
        break;
    }
  }

  /*
     if ( input == '1' ) {
     for (i = 255; i > 0 ; i--) {
     OCR1AL = i;
     printf("Hello world %u \r\n", i);
     _delay_ms(1000);
     }
     OCR1AL = 0;
     } else {

     printf("not the right answer");
     }
     */
  return 0;
}

void shake_sequence(uint8_t number) {
  uint8_t i;
  uint8_t j;

  pwm_init();

  OCR1AH = 0x50;
  OCR1AL = 0xff;

  PORTB |= _BV(ENA); // ON the enable bit
  for (j = 50 ; j > 0 ; j--) {
      OCR1AH = j;
    for (i = 255; i > 0 ; i--) {
      OCR1AL = i;
      printf("Hello world %u %u \r\n", i, j);
      _delay_ms(10);
    }
  }
     /*
  for (i = 0x00; i < 0x04; i++ )
  {
    printf("shake %u \r\n", i);
    PORTB |= _BV(DIR); // CW direction bit (high is CW) could use a toggle here
      for (i = 0xff; i > 0x00; i--) {
        OCR1AL = i;
        _delay_ms(10);
      }
      for (i = 0x00; i < 0xff ; i++) {
        OCR1AL = i;
        _delay_ms(10);
      }
  }
  */
  PORTD &= ~_BV(ENA);// OFF the enable bit
  TCCR1A &= ~_BV(COM1B0); // COM1B0 indicates COM-pare action toggling OCR1B (which is arduino pin 10) on Compare Match aka PWM.
}



/* functions left 
void find_first_well() () ; // TODO find the pwm speed for Servo.h 
void demo_centrifuge_stage() (); // TODO add control method for customization
void demo_full_sequence(); ()// TODO align sequence in a function
*/


void io_init (void) {
  //1 = output, 0 = input
  DDRB = _BV(PUL) | _BV(DIR) | _BV(ENA) | _BV(LED);
  //     PUL-Portb2 DIR-Portb3 ENA-Portb4 LED-light port 5 (13)
  DDRD = _BV(OPTO);
  //     Pin 2 of the arduino
  //     */
}

void pwm_init(void) {
  TCCR1A = _BV(COM1B0); // COM1B0 indicates COM-pare action toggling OCR1B (which is arduino pin 10) on Compare Match aka PWM.

  //  (0x81) TCCR1B ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10 136
  //  (0x80) TCCR1A COM1A1 COM1A0 COM1B1 COM1B0 – – WGM11 WGM10 134

  OCR1AH = 0x00; //this controls the higher half of the 16 bit "TOP" value


  TCCR1B = _BV(WGM02) | _BV(CS10); // the CS stuff sets the prescaler, 001 makes it no prescale (fastest clock)
  /*
  //   (0x81) TCCR1B ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10 136
  //    (0x80) TCCR1A COM1A1 COM1A0 COM1B1 COM1B0 – – WGM11 WGM10 134

  //there are three waveform-generation-mode bits: WGM03 WGM02 WGM01 WGM00 -- mode 3 (0100) is CTC mode (clear timer on compare match)

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
  1 1 1 External clock source on T1 pin. Clock on rising edgputchar
  */
}


void set_baud(void) {
  UBRR0H = (BRC >> 8);
  UBRR0L = BRC;

  //enables rx and tx, page 183 example from documentation
  UCSR0B = (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0 ) | (1 << TXCIE0);

  //198 of documentation, these two bits make it 8-bit
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  //since we don't use 2x
  UCSR0A &= ~(_BV(U2X0));

  //redirecting standard out and in to UART -- very clever tuupola
  stdout = &uart_output;
  stdin  = &uart_input;

}

void uart_putchar(char c, FILE *stream) {
  if (c == '\n') {
    uart_putchar('\r', stream);
  }
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
}


char uart_getchar(FILE *stream) {
  loop_until_bit_is_set(UCSR0A, RXC0);
  return UDR0;
}



