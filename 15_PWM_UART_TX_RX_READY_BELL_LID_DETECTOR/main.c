#include <util/delay.h>
#include <avr/io.h>
#include <stdint.h>

//this is for the lid detector interrupt kill safety feature
#include <avr/interrupt.h>

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

//these are some macros which help set the bits
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))


//Speaker Pin
#define SPEAKER PB0

//External Interrupt Pin
#define LID_DETECTOR PD3

//PUL  Portb2
//DIR  Portb3
//ENA  Portb4


#define PUL PB2
#define DIR PB3
#define ENA PB4
#define LED PB5

#define OPTO PD2

//for Speaker for loop
int spk;

//Uart setup
void uart_putchar(char c, FILE *stream);
char uart_getchar(FILE *stream);

//set up streams
FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);
FILE uart_input  = FDEV_SETUP_STREAM(NULL, uart_getchar, _FDEV_SETUP_READ);
//end uart setup


//unsigned long
int response_number;

//Define functions
//======================
void io_init(void);      // Initializes IO
void pwm_init(void);     // Initializes PWM
void set_baud(void);
void shake_sequence(uint8_t number);
void find_first_well(void); // TODO find the pwm speed for Servo.h 
void servoOn(int direction); // will act like the Servo.h PWM
void servoOff(void); // will act like the Servo.h PWM
void go_to_next_dot(void);
int check_if_there_is_a_dot_there(long duration_threshold);
void go_to_next_edge(void);
void play_ready_beep(void);
void set_external_interrupts(void);
//long pulseIn(int duration);


//demo sequences
void demo_centrifuge_stage(void); // TODO add control method for customization
void demo_full_sequence(void); // TODO align sequence in a function
//======================

int main (void) {
  io_init(); //Setup IO pins and defaults
  set_baud();

  char input;

  while (1) {

    input = getchar();

    //first stage will be a simple switch case`


    switch (input) {
      case '0':
        //shake sequence
        spk=0;
        play_ready_beep();

        putchar('0');
        break;
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
        demo_centrifuge_stage(); // TODO add control method for customization
        PORTB &= ~_BV(ENA);// OFF the enable bit
        putchar('2');
        break;
      case '3':
        //recording_sequence
        PORTB |= _BV(ENA); // ON the enable bit
        find_first_well(); // TODO find the pwm speed for Servo.h 
        //and emulate to avoid needing to perform trial and error
        PORTB &= ~_BV(ENA);// OFF the enable bit
        putchar('3');
        break;
      case '4':
        //full_sequence
        PORTB |= _BV(ENA); // ON the enable bit
        demo_full_sequence();// TODO create full sequence
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
        putchar('a');
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

  int time_delay = 10;
  uint8_t j_high = 255;
  uint8_t j_low  = 50;

  uint8_t i_high = 255;
  uint8_t i_low  = 0;

  int times;
  for ( times = 0 ; times < number ; times++)
  {
    for (j = j_high ; j > j_low ; j--) {
      OCR1AH = j;
      for (i = i_high; i > i_low ; i--) {
        OCR1AL = i;
        _delay_us(time_delay);
      }
    }
    for (j = j_low ; j < j_high ; j++) {
      OCR1AH = j;
      for (i = i_low; i < i_high; i++) {
        OCR1AL = i;
        _delay_us(time_delay);
      }
    }
    PORTB ^= _BV(DIR);

  }
  PORTD &= ~_BV(ENA);// OFF the enable bit
  TCCR1A &= ~_BV(COM1B0); // COM1B0 indicates COM-pare action toggling OCR1B (which is arduino pin 10) on Compare Match aka PWM.
}


void demo_centrifuge_stage() {
  uint8_t i;
  uint8_t j;

  pwm_init();

  OCR1AH = 0x50;
  OCR1AL = 0xff;

  PORTB |= _BV(ENA); // ON the enable bit

  int time_delay = 50;
  uint8_t j_high = 255;
  uint8_t j_low  = 40;

  uint8_t i_high = 255;
  uint8_t i_low  = 0;

  for (j = j_high ; j > j_low ; j--) {
    OCR1AH = j;
    for (i = i_high; i > i_low ; i--) {
      OCR1AL = i;
      _delay_us(time_delay);
    }
  }
  _delay_ms(4000);
  PORTD &= ~_BV(ENA);// OFF the enable bit
  TCCR1A &= ~_BV(COM1B0); // COM1B0 indicates COM-pare action toggling OCR1B (which is arduino pin 10) on Compare Match aka PWM.

}

// TODO add control method for customization

void find_first_well() {
  //first we make sure we're on an open space
  long open_space=0;//automatically intialized to zero
  //printf("looking for a good start\n");
  servoOn(1); //1 sets DIR to clockwise
  do {
    _delay_us(100);
    if ( PIND & _BV(PD2) ) {
      open_space += 100; //more open_space 
      // printf("%ld \n", open_space);
    } else {
      open_space = 0;  //if no open space then reset counter
      // printf("%ld \n", open_space);
    }
  } while (open_space < 400000L);

  servoOff();
//  printf("let us begin our adventure\n");

  //_delay_ms(1000);

  //begin first dot sequence


  servoOn(1);
  int counter = 0;
  while (counter < 1000)
  {

    if ( PIND & _BV(PD2) ) {
      _delay_us(10); // this is what happens if nothing detected. (detection events give low value)
      counter = 0;
    } else {
      counter++;
      //printf("counter is %d \n", counter); // don't use printf it slows everything down ruining timing measurement
    }
  }
  servoOff();
//  printf("what do we have here -- a well?\n");

  //_delay_ms(1000);
  go_to_next_dot(); //begin counting dots
}

void go_to_next_dot() {
  int count_dots=0;
  int last_dot=0;

  int there_is_a_dot=3;

  while (last_dot==0) { //wait why 11? oh if misaligned and detects too many dots
    //look for dot, but with a time limit

//    printf("there is a dot preresult %d \n", there_is_a_dot);
    servoOn(1);
    there_is_a_dot =  check_if_there_is_a_dot_there( 500000L ); //DONE --  replaced this with a check_if_there_is_a_dot_there function
    //servoOff();
    //_delay_ms(1000);
    //printf("there is a dot result %d \n", there_is_a_dot);
    //printf("how many dots %d \n", count_dots);

    if (there_is_a_dot == 1)
    {
      count_dots++;
      //printf("how many dots %d \n", count_dots);
      //go to space right after the dot then begin the loop again
      go_to_next_edge();
      //printf("how many dots %d \n", count_dots);
    } else if (there_is_a_dot==0) {
      servoOff(); // stop
      _delay_ms(100); //wait for stop
      servoOn(0); // go reverse
      //printf("is this the real mistake? %d\n", count_dots);
      check_if_there_is_a_dot_there( 1000000L ); //check for the dot
      servoOff(); // found the dot, next we will go a bit further to center pic
      //_delay_ms(1000);
      //printf("found the edge, hopefully not more dots %d\n", count_dots);

      servoOn(0);
      _delay_ms(600); // hopefully this is long enough
      servoOff();
      last_dot = 1; //break loop as we found last dot
    } else {
      //printf("count them dots %d\n", count_dots);
    }
  }

  // send back the number of dots to pythonSerial.py
  switch (count_dots) {
    case 1:
      printf("%d\r\n", 7);
      break;
    case 2:
      printf("%d\r\n", 8);
      break;
    case 3:
      printf("%d\r\n", 9);
      break;
    case 4:
      printf("%d\r\n", 10);
      break;
    default:
      break;
  }

  PORTB |= _BV(LED); // TODO Test LED ON
  _delay_ms(2000); // wait for camera to take photo
  PORTB &= ~_BV(LED); // TODO Test LED OFF
  servoOn(1);
  _delay_ms(1000);

  count_dots = 0; // reset our global dot counter for next well
}


int check_if_there_is_a_dot_there ( long duration_threshold ) {
  int evidence_of_dot = 0;
  long open_space = 0; //counter for open space
  do {
    _delay_us(100);
    if ( PIND & _BV(PD2) ) {
      open_space += 100; //more open_space 
      // printf("%ld \n", open_space);
      evidence_of_dot = 0; //reset counter to filter false positives
    } else {
      evidence_of_dot++; // gather evidence of a dot
      if ( evidence_of_dot > 10) {
        return 1; //there is a dot
      }
    }
  } while (open_space < duration_threshold);

  return 0; //no dot detected within temporal threshold

}

void go_to_next_edge(void) {

  servoOn(1);
  int counter = 0;
  while (counter < 10)
  {

    if ( !(PIND & _BV(PD2)) ) {
      _delay_ms(1); // this is what happens if nothing detected. (detection events give low value)
      counter = 0;
    } else {
      counter++;
      //printf("counter is %d \n", counter); // don't use printf it slows everything down ruining timing measurement
    }
  }
  servoOff();

}

void servoOn(int direction) {
  // servo speed is 50Hz
  // this is approximated by the 16 bit register as 
  // UCR1AH 78 and UCR1AL 30 ( although this is at 50% duty cycle, thankfully the motor controller is edge triggered

  //TODO do an if statement so this isn't redundant (for cleanup purposes only)
  pwm_init();

  TCCR1B = _BV(WGM02) | _BV(CS11); // the CS stuff sets the prescaler, 010 makes it factor of 2 prescale (second fastest clock 8MHz) WGM02 makes it so that compare register actually does something

  //these two numbers give the 50.0Hz frequency simalcrum of Servo.h
  OCR1AH = 78;
  OCR1AL = 30;
  if (direction == 1) {
    PORTB |= _BV(DIR);
  } else {
    PORTB &= ~_BV(DIR);
  }
  PORTB |= _BV(ENA); // ON the enable bit


}

void servoOff(void){

  PORTD &= ~_BV(ENA);// OFF the enable bit
  TCCR1A &= ~_BV(COM1B0); // COM1B0 indicates COM-pare action toggling OCR1B (which is arduino pin 10) on Compare Match aka PWM.

  //reset the original prescaler
  TCCR1B = _BV(WGM02) | _BV(CS10); // the CS stuff sets the prescaler, 001 makes it no prescale (fastest clock)
}

//long pulseIn()

void demo_full_sequence(void) {
        shake_sequence(2); // TODO shake twice -- can mod this later for custom shakes
        demo_centrifuge_stage(); // TODO add control method for customization
        find_first_well(); // finds first well 
        putchar('3'); // finds other wells -- see python script
}


void io_init (void) {
  //1 = output, 0 = input5
  DDRB = _BV(PUL) | _BV(DIR) | _BV(ENA) | _BV(LED) | _BV(SPEAKER);
  //     PUL-Portb2 DIR-Portb3 ENA-Portb4 LED-light port 5 (13)
  DDRD = ~_BV(OPTO); //OPTO is an input
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



void play_ready_beep() {

        int eighth_note = 120;
        int quarter_note = eighth_note*2;
        int half_note = eighth_note*4;
     //   int whole_note=  eighth_note*8;

        for (spk = 0; spk < eighth_note; spk++) {
          PORTB ^=  _BV(SPEAKER); //bitwise xor that toggles pins-- this is really useful!!
          _delay_us(2000); // should give around 400Hz
        }
        for (spk = 0; spk < eighth_note; spk++) {
          PORTB ^=  _BV(SPEAKER); //bitwise xor that toggles pins-- this is really useful!!
          _delay_us(1600); // 2000*4/5   = 1600 or perfect third
        }
        for (spk = 0; spk < eighth_note; spk++) {
          PORTB ^=  _BV(SPEAKER); //bitwise xor that toggles pins-- this is really useful!!
          _delay_us(1333); // 2000*4/5*5/6 = 1333 or perfect fifth
        }
        for (spk = 0; spk < quarter_note; spk++) {
          PORTB ^=  _BV(SPEAKER); //bitwise xor that toggles pins-- this is really useful!!
          _delay_us(1000); //2000*4/5*5/6*6/8 = 1000  one octave
        }
}

// strategy set external interrupts when doing something dangerous
// TODO will make sure to release this after dangerous sequences

void set_external_interrupts () {
  DDRD = _BV(LID_DETECTOR);
  //PORTD = _BV(LID_DETECTOR); // may not be necessary, I forget if the lid detector is active high or active low

  GICR = _BV(INT1); //enable INT1 I assume as not INT0
  MCUCR = _BV(ISC11) | _BV(ISC10); // need to double check these

  sei(); // literally means set external interrupts
}


//external interrupt is programmed to buzz when triggered
ISR(INT0_vect) {
        for (spk = 0; spk < 400; spk++) {
          PORTB ^=  _BV(SPEAKER); //bitwise xor that toggles pins-- this is really useful!!
          _delay_us(2000); // should give around 400Hz
        }
}
