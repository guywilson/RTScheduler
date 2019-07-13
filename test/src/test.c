#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "sched_utils.h"
#include "../../sched/src/scheduler.h"

void testTask1(PTASKPARM p)
{
    printf("In test task 1\n");
}

void testTask2(PTASKPARM p)
{
    printf("In test task 2\n");
}

void testTask3(PTASKPARM p)
{
    printf("In test task 3\n");
}

void testTask4(PTASKPARM p)
{
    printf("In test task 4\n");
}

int main(void)
{
    PTASKDESC       task;

    initSchedUtils();

    initScheduler();

    registerTask(0x0100, &testTask1);
    registerTask(0x0200, &testTask2);
    registerTask(0x0300, &testTask3);
    registerTask(0x0400, &testTask4);

    scheduleTask(0x0100, 1000, TASK_PRIORITY_NORMAL, NULL);
    scheduleTask(0x0200, 800, TASK_PRIORITY_NORMAL, NULL);
    scheduleTask(0x0300, 2000, TASK_PRIORITY_NORMAL, NULL);
    scheduleTask(0x0400, 1000, TASK_PRIORITY_HIGH, NULL);

    task = getScheduledTasks();

    while (task != NULL) {
        printf("Listing task details\n");
        printf("Task ID: 0x%04X\n", task->ID);
        printf("Start time: %d\n", task->startTime);
        printf("Scheduled time: %d\n", task->scheduledTime);
        printf("Requested delay: %d\n", task->delay);
        printf("Priority: %d\n", (uint8_t)task->priority);
        printf("IsAllocated: %s\n", (task->isAllocated == 1 ? "true" : "false"));
        printf("IsScheduled: %s\n", (task->isScheduled == 1 ? "true" : "false"));
        printf("\n\n");

        task = task->next;
    }

    printf("Starting scheduler...\n");

    schedule();
}