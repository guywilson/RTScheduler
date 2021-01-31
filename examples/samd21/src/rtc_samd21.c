#include <stdint.h>

#include <Arduino.h>
#include <samd.h>
#include <scheduler.h>

#include "rtc_samd21.h"
#include "led_utils.h"

extern void _rtcISR();

int sysTickHook()
{
	_rtcISR();
	
	return 0;
}
