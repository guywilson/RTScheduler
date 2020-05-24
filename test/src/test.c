#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include "sched_utils.h"
#include "../../sched/src/scheduler.h"
#include "taskdef.h"


void wdtTask(PTASKPARM p)
{
    printf("In wdt task\n");
}

void heartbeatTask(PTASKPARM p)
{
	static uint8_t on = 0;

    if (on) {
        on = 0;
        printf("Turn off... \n");
		scheduleTaskOnce(TASK_HEARTBEAT, 950, NULL);
    }
    else {
        on = 1;
        printf("Turn on...\n");
		scheduleTaskOnce(TASK_HEARTBEAT, 50, NULL);
    }
}

void adcTask(PTASKPARM p)
{
    //printf("In adc task\n");
}

void anemometerTask(PTASKPARM p)
{
    printf("In anemometer task\n");
}

void rainGaugeTask(PTASKPARM p)
{
    printf("In rain gauge task\n");
}

void rxTask(PTASKPARM p)
{
    printf("In rx task\n");
}

int main(void)
{
    int             loop = 1;
    PTASKDESC       task;

    initSchedUtils();

    initScheduler(6);

	registerTask(TASK_WDT, &wdtTask);
	registerTask(TASK_HEARTBEAT, &heartbeatTask);
	registerTask(TASK_ADC, &adcTask);
	registerTask(TASK_ANEMOMETER, &anemometerTask);
	registerTask(TASK_RAINGUAGE, &rainGaugeTask);
	registerTask(TASK_RXCMD, &rxTask);

	scheduleTask(
			TASK_WDT,
			250,
 			NULL);

	scheduleTaskOnce(
			TASK_HEARTBEAT,
			3,
			NULL);

	scheduleTask(
			TASK_ANEMOMETER,
			1000,
			NULL);

	scheduleTask(
			TASK_RAINGUAGE,
			3600000,
			NULL);

    task = getRegisteredTasks();

    while (loop) {
        if (isLastTask(task)) {
            loop = 0;
        }

        printf("Listing task details\n");
        printf("Task ID: 0x%04X\n", task->ID);
        printf("Start time: %d\n", task->startTime);
        printf("Scheduled time: %d\n", task->scheduledTime);
        printf("Requested delay: %d\n", task->delay);
        printf("IsAllocated: %s\n", (task->isAllocated == 1 ? "true" : "false"));
        printf("IsScheduled: %s\n", (task->isScheduled == 1 ? "true" : "false"));
        printf("\n\n");

        task = task->next;
    }

    triggerADC();
    
    printf("Starting scheduler...\n");

    schedule();
}