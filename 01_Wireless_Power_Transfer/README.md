Wireless Power Transfer
=======================

Wireless power transfer requires as high a frequency oscillation as you can muster.

In order to create the pulse (without blocking), we will need to use the PWM timers of the Atmega328p.

## Making your first Makefile

//TODO create tutorial for creating a simple makefile on the computers available

## The process for creating fast code

### One Byte at a Time

DDRB = 0b00001111
PORTB = 0b00001111

### Bit math

Shift operator && Convenient Macro usage
bitwise or
bitwise xor
bitwise and
