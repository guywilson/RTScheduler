#include <stdint.h>
#include <samd.h>
#include <variant.h>

#include "led_utils.h"

void setupLEDPin(void)
{
	/*
	** Set Port B - Pin 5 as output
	** On the Arduino Nano, this is connected to the
	** onboard LED...
	*/
    REG_PORT_DIRSET0 = (1 << LED_BUILTIN);
}

void turnOn(uint32_t LED_ID)
{
	/* set pin 5 high to turn led on */
	REG_PORT_OUTSET0 = (1 << LED_ID);
}

void turnOff(uint32_t LED_ID)
{
	/* set pin 5 low to turn led off */
	REG_PORT_OUTCLR0= (1 << LED_ID);
}

void toggleLED(uint32_t LED_ID)
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
