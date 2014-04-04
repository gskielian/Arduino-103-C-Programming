#include <avr/io.h>
#include <stdio.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif


#ifndef BAUD
#define BAUD 9600
#endif

#include <util/setbaud.h>


void uart_init(void) {


}
