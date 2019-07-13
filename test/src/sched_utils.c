#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#include "../../sched/src/scheduler.h"
#include "sched_utils.h"

pthread_t			tidISR;

/*
** Simulate timer interrupt...
*/
void * timerISRThread(void * pArgs)
{
    while (1) {
        _rtcISR();

        usleep(250);
    }
}

void initSchedUtils()
{
	int				err;

	/*
	 * Start cmd thread...
	 */
	err = pthread_create(&tidISR, NULL, &timerISRThread, NULL);

	if (err != 0) {
		printf("ERROR! Can't create txCmdThread() :[%s]\n", strerror(err));
		exit(-1);
	}
	else {
		printf("Thread txCmdThread() created successfully\n");
	}
}

void handleError(uint16_t errorCode)
{
    printf("Got scheduler error [0x%04X]\n", errorCode);
}
