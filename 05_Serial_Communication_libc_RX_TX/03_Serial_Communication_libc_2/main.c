#include <avr/interrupt.h>

#define F_CPU 16000000UL
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1 )
#define TX_BUFFER_SIZE 128

#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

char serialBuffer[TX_BUFFER_SIZE];
uint8_t serialReadPos; //automatically init to zero, saves space not to write it
uint8_t serialWritePos; //in priniciple, saving space is high priority in atmel 
                        //so it is good to get into the practice



//additional methods
void appendSerial(char c);
void serialWrite(char c[]);

int main(void) {
  UBRR0H = (BRC >> 8); //gets shifted into the upper bytess
  UBRR0L = BRC;        // puts it into the lower register

  //SETTING UP THE TRANSMITTER
  //
  //there are three uart control registers A, B, C
  //
  //SETTING UP B
  //B has the transmitter enable
  UCSR0B = (1 << TXEN0) | (1 << TXCIE0); //need the second one for the USART_TX_vect

  //SETTING UP C
  //we will need to set up the character size
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

  //Data Register UDR0 == USART I/O Data Register n
  //once you place data into this register it already starts transmitting

  sei();

  serialWrite("HELLO\n\r");
  serialWrite("World\n\r");

  _delay_ms(1500);

  serialWrite("Goodbye\n\r");

  while (1)
  {
  }
}


void appendSerial(char c) {
  serialBuffer[serialWritePos] = c;
  serialWritePos++;
  if(serialWritePos >= TX_BUFFER_SIZE)
  {
    serialWritePos = 0;
  }
}

//works off Bit 6 Transmit complete TXCN
// need to set TXCIE0  in order to use that Vector
ISR(USART_TX_vect) { //interrupt vector is USART_TX_vect works off transmit complete
  if(serialReadPos != serialWritePos)
  {
    UDR0 = serialBuffer[serialReadPos];
    serialReadPos++;

      if(serialReadPos >= TX_BUFFER_SIZE)
      {
        serialReadPos=0;
      }
  }
}


void serialWrite(char c[]) {
  for(uint8_t i = 0; i < strlen(c); i++)
  {
    appendSerial(c[i]);
  }

  if(UCSR0A & (1 << UDRE0)) //in control register a flag indicated if UDRn is ready to receive new data, if so then we send it a character 0 in order to reset everthing
  {
    UDR0 = 0;
  }
}
