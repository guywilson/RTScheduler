#include <avr/io.h>

#ifndef _INCL_LEDUTILS
#define _INCL_LEDUTILS

#define		LED_PORT0			_BV(PORTB0)
#define		LED_PORT1			_BV(PORTB1)
#define		LED_PORT2			_BV(PORTB2)
#define		LED_PORT3			_BV(PORTB3)
#define		LED_PORT4			_BV(PORTB4)
#define		LED_PORT5			_BV(PORTB5)

#define		LED_ONBOARD			LED_PORT5

void		setupLEDPin(void);
void		turnOn(int LED_ID);
void		turnOff(int LED_ID);
void 		toggleLED(int LED_ID);

#endif