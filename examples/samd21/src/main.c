#include <stddef.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <scheduler.h>
#include <schederr.h>

#include "taskdef.h"

#include "rtc_atmega328p.h"
#include "heartbeat.h"
#include "led_utils.h"

void main(void) __attribute__ ((noreturn));

void setup(void)
{
	setupLEDPin();
	setupRTC();
}

#pragma GCC diagnostic ignored  "-Wmain"
void main(void)
{
	/*
	 * Disable interrupts...
	 */
	cli();

	setup();

	/*
	 * Enable interrupts...
	 */
    sei();
	
	initScheduler(1);

	registerTask(TASK_HEARTBEAT, &HeartbeatTask);

	scheduleTaskOnce(
			TASK_HEARTBEAT,
			rtc_val_ms(950),
			NULL);

	/*
	** Start the scheduler...
	*/
	schedule();
}
