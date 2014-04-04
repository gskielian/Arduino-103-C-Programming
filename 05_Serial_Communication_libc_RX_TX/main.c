#include <avr/io.h>



//Note: I hope this is compatible with the hardware interrupt scheme
#include <avr/interrupt.h>



//using this for pause hacks
#include <util/delay.h>

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


//from the tx tutorial, may need to remove these
char serialBuffer[TX_BUFFER_SIZE];
uint8_t serialReadPos;            //auto inits to zero saves a little space not to include
uint8_t serialWritePos;           //auto inits to zero saves a little space not to include


char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxReadPos;              //auto inits to zero saves a little space not to include
uint8_t rxWritePos;             //auto inits to zero saves a little space not to include



//prototypes TX
void appendSerial(char c);
void serialWrite(char c[]);

//prototypes RX
char peekChar(void);
char getChar(void);


int main(void) {

  //baud rate I think
  UBRR0H = (BRC >> 8); // this is for setting the baud rate
  UBRR0L = BRC;        // like a barabara it is cut in half

  //enables rx and tx, page 183 example from documentation
  UCSR0B = (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0 ) | (1 << TXCIE0);

  //198 of documentation, these two bits make it 8-bit
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  //sets the direction of the DDRB to be an OUTPUT
  DDRB =   (1 << PORTB0);

  //Enables serial communication
  sei();

  while(1) {
    char c = getChar();

    if(c == '1') {
      sbi(PORTB, PORTB0);
      serialWrite(c);
    }
    else if (c == '0') {
      cbi(PORTB, PORTB0);
      serialWrite(c);
    }


  }
} // end int main


//TX Methods


//append serial add things to the buffer
void appendSerial(char c) {
  serialBuffer[serialWritePos] = c;
  serialWritePos++;
  if(serialWritePos >= TX_BUFFER_SIZE)
  {
    serialWritePos = 0;
  }
}



//like for stack, this doesn't pop off the character
char peekChar(void) {
  char ret = '\0';

  if(rxReadPos != rxWritePos) {
    ret = rxBuffer[rxReadPos];
  }

  return ret;
} // end peekChar


//this pops the character
char getChar(void) {

  char ret = '\0';

  if(rxReadPos != rxWritePos) {

    ret = rxBuffer[rxReadPos];
    rxReadPos++;

    if(rxReadPos >= RX_BUFFER_SIZE) {
      rxReadPos=0;
    }

  }

  return ret;
} // end getChar

//interrupts
ISR(USART_RX_vect) {
  rxBuffer[rxWritePos] = UDR0;// anything your read from it is received, anything written is transmitted

  rxWritePos++;

  if(rxWritePos >= RX_BUFFER_SIZE) {
    rxWritePos = 0;
  }
} // end ISR
