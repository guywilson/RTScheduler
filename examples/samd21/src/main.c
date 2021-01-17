#include <stddef.h>
#include <samd.h>

#include <scheduler.h>
#include <schederr.h>

#include "taskdef.h"

#include "rtc_samd21.h"
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
	setup();
	
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
