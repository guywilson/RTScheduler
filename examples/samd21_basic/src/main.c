#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <Arduino.h>
#include <samd.h>
#include <variant.h>

volatile uint32_t 			_realTimeClock = 0;		// The real time clock counter
volatile int				_state = 0;

int main(void)
{
	init();
	initVariant();

	SysTick_Config(SystemCoreClock / 1000);
	
	pinMode(LED_BUILTIN, OUTPUT);
	
	while (1) {
        delay(50);
	}
	
	return 0;
}

int sysTickHook()
{
	if (_realTimeClock == 100) {
		if (_state) {
			digitalWrite(LED_BUILTIN, LOW);
			_state = 0;
		}
		else {
			digitalWrite(LED_BUILTIN, HIGH);
			_state = 1;
		}

		_realTimeClock = 0;
	}

	/*
	** The RTC is incremented every 1 ms,
	** it is used to drive the real time clock
	** for the scheduler...
	*/
	_realTimeClock++;
	
	return 0;
}
