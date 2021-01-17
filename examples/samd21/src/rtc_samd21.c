#include <stdint.h>
#include <samd.h>
#include <scheduler.h>

#include "rtc_samd21.h"

void setupRTC()
{
	SysTick_Config(SystemCoreClock / 1000);
}

void SysTick_Handler()
{
	_rtcISR();
}
