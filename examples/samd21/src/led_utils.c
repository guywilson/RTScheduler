#include <stdint.h>

#include <Arduino.h>
#include <samd.h>
#include <variant.h>

#include "led_utils.h"

void setupLEDPin(void)
{
	pinMode(LED_BUILTIN, OUTPUT);
}

void turnOn()
{
	digitalWrite(LED_BUILTIN, HIGH);
}

void turnOff()
{
	digitalWrite(LED_BUILTIN, LOW);
}

void toggleLED()
{
	static unsigned char state = 0;
	
	if (!state) {
		turnOn();
		state = 1;
	}
	else {
		turnOff();
		state = 0;
	}
}
