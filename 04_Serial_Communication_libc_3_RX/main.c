#include <avr/io.h>
#include <avr/interrupt.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))

#define F_CPU 16000000UL
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1)

#define RX_BUFFER_SIZE 128

char rxBuffer[RX_BUFFER_SIZE];
uint8_t rxReadPos = 0;
uint8_t rxWritePos = 0;


char peekChar(void);
char getChar(void);

int main(void) {

  //baud rate I think
  UBRR0H = (BRC >> 8);
  UBRR0L = BRC;

  //enables rx and tx, page 183 example from documentation
  UCSR0B = (1 << RXEN0) | (1 << RXCIE0) | (1 << TXEN0 );

  //198 of documentation, these two bits make it 8-bit
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  DDRB =   (1 << PORTB0);

  sei();

  while(1) {
    char c = getChar();

    if(c == '1') {
      sbi(PORTB, PORTB0);
    }
    else if (c == '0') {
      cbi(PORTB, PORTB0);
    }
  }
} // end int main



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
