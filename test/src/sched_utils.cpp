#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

extern "C" {
#include "../../sched/src/scheduler.h"
#include "taskdef.h"
}
#include "sched_utils.h"
#include "logger.h"

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

		scheduleTaskOnce(TASK_RXCMD, RUN_NOW, NULL);
	}
}

void triggerADC()
{
	scheduleTaskOnce(TASK_ADC, 15, NULL);
}

void initSchedUtils()
{
	int				err;
	int				defaultLoggingLevel = LOG_LEVEL_DEBUG | LOG_LEVEL_INFO | LOG_LEVEL_ERROR | LOG_LEVEL_FATAL;

	Logger & log = Logger::getInstance();

	log.initLogger(defaultLoggingLevel);

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
