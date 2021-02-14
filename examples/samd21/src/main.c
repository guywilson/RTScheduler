#include <stddef.h>

#include <Arduino.h>
#include <samd.h>
#include <variant.h>

#include <scheduler.h>
#include <schederr.h>

#include "taskdef.h"

#include "rtc_samd21.h"
#include "heartbeat.h"
#include "led_utils.h"

extern void __libc_init_array(void);

int main(void)
{
 	init();
    __libc_init_array();
     initVariant();

	setupLEDPin();
	
	initScheduler(16);
	
	registerTask(TASK_HEARTBEAT, &HeartbeatTask);

	scheduleTaskOnce(
			TASK_HEARTBEAT,
			rtc_val_ms(950),
			NULL);

	/*
	** Start the scheduler...
	*/
	schedule();
	
	return -1;
}
