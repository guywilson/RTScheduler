#include <stddef.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <samd.h>

#include <scheduler.h>
#include <schederr.h>

#include "taskdef.h"

#include "rtc_samd21.h"
#include "heartbeat.h"
#include "led_utils.h"

#undef errno
extern int errno;
extern int _end;

void main(void) __attribute__ ((noreturn));

void _exit(int status)
{
	while (1);
}

caddr_t _sbrk(int incr)
{
	static unsigned char *heap = NULL;
	unsigned char *prev_heap;

	if (heap == NULL) {
		heap = (unsigned char *)&_end;
	}
	prev_heap = heap;

	heap += incr;

	return (caddr_t) prev_heap;
}

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
