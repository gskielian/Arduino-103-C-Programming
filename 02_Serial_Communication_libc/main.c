#define F_CPU 16000000UL
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1 )

#include <avr/io.h>
#include <util/delay.h>


int main(void) {
  UBRR0H = (BRC >> 8); //gets shifted into the upper bytess
  UBRR0L = BRC;        // puts it into the lower register

  //SETTING UP THE TRANSMITTER
  //
  //there are three uart control registers A, B, C
  //
  //SETTING UP B
  //B has the transmitter enable
  UCSR0B = (1 << TXEN0);

  //SETTING UP C
  //we will need to set up the character size
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
  

  //Data Register UDR0 == USART I/O Data Register n
  //once you place data into this register it already starts transmitting

  while (1)
  {
    UDR0 ='8';
    _delay_ms(1000);
  }
}

