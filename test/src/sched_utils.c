#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#include "../../sched/src/scheduler.h"
#include "sched_utils.h"
#include "taskdef.h"

pthread_t			tidISR;
pthread_t			tidADC;
pthread_t			tidRx;

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

void * rxISRThread(void * pArgs)
{
	uint32_t		idle;
	uint32_t		busy;
	double			cpuPct = 0.0;

	while (1) {
		sleep(1);

		// getCPURatio(&idle, &busy);
		// cpuPct = ((double)busy * 100.0) / ((double)idle - (double)busy);
		// printf("CPU = %d/%d (%.2f%%)\n", busy, (idle - busy), cpuPct);

		scheduleTask(TASK_RXCMD, RUN_NOW, NULL);
	}
}

void triggerADC()
{
	scheduleTask(TASK_ADC, 15, NULL);
}

void initSchedUtils()
{
	int				err;

	/*
	** Start timer ISR thread...
	*/
	err = pthread_create(&tidISR, NULL, &timerISRThread, NULL);

	if (err != 0) {
		printf("ERROR! Can't create timerISRThread() :[%s]\n", strerror(err));
		exit(-1);
	}
	else {
		printf("Thread timerISRThread() created successfully\n");
	}

	/*
	** Start RX ISR thread...
	*/
	err = pthread_create(&tidRx, NULL, &rxISRThread, NULL);

	if (err != 0) {
		printf("ERROR! Can't create rxISRThread() :[%s]\n", strerror(err));
		exit(-1);
	}
	else {
		printf("Thread rxISRThread() created successfully\n");
	}
}

void handleError(uint16_t errorCode)
{
    printf("Got scheduler error [0x%04X]\n", errorCode);
	exit(-1);
}
