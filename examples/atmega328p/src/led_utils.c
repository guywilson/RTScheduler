#include <avr/io.h>

#include "led_utils.h"

void setupLEDPin(void)
{
	/*
	** Set Port B - Pin 5 as output
	** On the Arduino Nano, this is connected to the
	** onboard LED...
	*/
    DDRB |= _BV(DDB5);

	/*
	** Set Port B - Pin 0 as output
	** Use a secondary external LED for testing...
	*/
    DDRB |= _BV(DDB0);
}

void turnOn(int LED_ID)
{
	/* set pin 5 high to turn led on */
	PORTB |= LED_ID;
}

void turnOff(int LED_ID)
{
	/* set pin 5 low to turn led off */
	PORTB &= ~LED_ID;
}

void toggleLED(int LED_ID)
{
	static unsigned char state = 0;
	
	if (!state) {
		turnOn(LED_ID);
		state = 1;
	}
	else {
		turnOff(LED_ID);
		state = 0;
	}
}
