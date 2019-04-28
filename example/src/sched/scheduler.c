/******************************************************************************
**
** File: scheduler.c
**
** Description: API functions for the real-time scheduler. Inspired by a
** distant memory of a scheduler I used circa 1996
**
** Copyright: Guy Wilson (c) 2018
**
******************************************************************************/

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "scheduler.h"
#include "schederr.h"

#define CHECK_TIMER_OVERFLOW

/******************************************************************************
**
** The TASKDEF struct.
**
** Holds the per-task info for the scheduler.
**
******************************************************************************/
typedef struct
{
	uint16_t		ID;				// Unique user-assigned ID
	timer_t			startTime;		// The RTC value when scheduleTask() was callled
	timer_t			scheduledTime;	// The RTC value when the task should run
	timer_t			delay;			// The requested delay (in ms) of when the task should run
	uint8_t			isScheduled;	// Is this task scheduled
	uint8_t			isAllocated;	// Is this allocated to a task
	PTASKPARM		pParameter;		// The parameters to the task
	
	void (* run)(PTASKPARM);		// Pointer to the task function to run
}									// Must be of the form void task(PTASKPARM p);
TASKDEF;

typedef TASKDEF *	PTASKDEF;

TASKDEF		taskDefs[MAX_TASKS];			// Array of tasks for the scheduler
int			taskCount = 0;					// Number of tasks registered

/******************************************************************************
**
** Name: _nullTask()
**
** Description: Null task assigned to TASKDEF entries that are unused.
**
** Parameters:	PTASKPARM	p		Unused.
**
** Returns:		void 
**
******************************************************************************/
void _nullTask(PTASKPARM p)
{
	handleError(ERROR_SCHED_NULLTASKEXEC);
}

/******************************************************************************
**
** Name: _getScheduledTime()
**
** Description: Calculates the future RTC value when the task should run
**
** Parameters:
**				timer_t		startTime		The RTC value when called
**				timer_t		requestedDelay	The delay in ms before the task runs
**
** Returns: 
**				timer_t		The future RTC value when the task should run
**
** If we don't care about checking if the timer will overflow, use a macro
** to calculate the default scheduled time (with risk of overflow). 
**
** With a timer incrementing every 1ms:
**
** A 32-bit timer will overflow in approximately 50 days. 
** A 64-bit timer will overflow in approximately 585 million years!
**
******************************************************************************/
#ifdef CHECK_TIMER_OVERFLOW
timer_t _getScheduledTime(timer_t startTime, timer_t requestedDelay)
{
	timer_t			overflowTime;
	
	overflowTime = MAX_TIMER_VALUE - startTime;
	
	if (overflowTime < requestedDelay) {
		return (requestedDelay - overflowTime);
	}
	else {
		return (startTime + requestedDelay);
	}
}
#else
#define _getScheduledTime(startTime, requestedDelay)	(startTime + requestedDelay)
#endif

/******************************************************************************
**
** Public API functions
**
******************************************************************************/

/******************************************************************************
**
** Name: initScheduler()
**
** Description: Initialises the scheduler, must be called before any other
** scheduler API functions.
**
** Parameters:	N/A
**
** Returns:		void 
**
******************************************************************************/
void initScheduler()
{
	int			i = 0;
	PTASKDEF	td = NULL;
	
	taskCount = 0;
	
	for (i = 0;i < MAX_TASKS;i++) {
		td = &taskDefs[i];
		
		td->ID				= 0;
		td->startTime		= 0;
		td->scheduledTime	= 0;
		td->delay			= 0;
		td->isScheduled		= 0;
		td->isAllocated		= 0;
		td->pParameter		= NULL;
		td->run				= &_nullTask;
	}
}

/******************************************************************************
**
** Name: registerTask()
**
** Description: Registers a task with the scheduler.
**
** Parameters:	uint16_t	taskID		A unique ID for the task
**				void 		(* run)		Pointer to the actual task function
**
** Returns:		void 
**
******************************************************************************/
void registerTask(uint16_t taskID, void (* run)(PTASKPARM))
{
	int			i = 0;
	PTASKDEF	td = NULL;
	uint8_t		noFreeTasks = 1;
	
	for (i = 0;i < MAX_TASKS;i++) {
		td = &taskDefs[i];
		
		if (!td->isAllocated) {
			td->ID = taskID;
			td->isAllocated = 1;
			td->run = run;
			taskCount++;
			noFreeTasks = 0;
			break;
		}
	}
    
	if (noFreeTasks) {
		handleError(ERROR_SCHED_NOFREETASKS);
	}
	if (taskCount > MAX_TASKS) {
		handleError(ERROR_SCHED_TASKCOUNTOVERFLOW);
	}
}

/******************************************************************************
**
** Name: deregisterTask()
**
** Description: De-registers a task with the scheduler, freeing up the task
** definition.
**
** Parameters:	uint16_t	taskID		The unique ID for the task
**
** Returns:		void 
**
******************************************************************************/
void deregisterTask(uint16_t taskID)
{
	int			i = 0;
	PTASKDEF	td = NULL;
	
	for (i = 0;i < MAX_TASKS;i++) {
		td = &taskDefs[i];
		
		if (td->ID == taskID) {
			td->ID				= 0;
			td->startTime		= 0;
			td->scheduledTime	= 0;
			td->delay			= 0;
			td->isScheduled		= 0;
			td->isAllocated		= 0;
			td->pParameter		= NULL;
			td->run				= &_nullTask;
			
			taskCount--;
			break;
		}
	}
}

/******************************************************************************
**
** Name: scheduleTask()
**
** Description: Schedules the task to run after the specified delay. A task
** must be registered using registerTask() before it can be scheduled.
**
** Parameters:	
** uint16_t		taskID		The unique ID for the task
** timer_t		time		Number of ms in the future for the task to run
** PTASKPARM	p			Pointer to the task parameters, can be NULL
**
** Returns:		void 
**
******************************************************************************/
void scheduleTask(uint16_t taskID, timer_t time, PTASKPARM p)
{
	int			i = 0;
	PTASKDEF	td = NULL;
	
	for (i = 0;i < MAX_TASKS;i++) {
		td = &taskDefs[i];
		
		if (td->ID == taskID) {
			td->startTime = _realTimeClock;
			td->delay = time;
			td->scheduledTime = _getScheduledTime(td->startTime, td->delay);
			td->isScheduled = 1;
			td->pParameter = p;
			break;
		}
	}
}

/******************************************************************************
**
** Name: rescheduleTask()
**
** Description: Reschedules the task to run again after the same delay as
** specified in the scheduleTask() call. Useful for calling at the end of
** the task itself to force it to run again (after the specified delay).
**
** If you want to reschedule the task to run after a different delay, simply
** call scheduleTask() instead.
**
** Parameters:	
** uint16_t		taskID		The unique ID for the task
** PTASKPARM	p			Pointer to the task parameters, can be NULL
**
** Returns:		void 
**
******************************************************************************/
void rescheduleTask(uint16_t taskID, PTASKPARM p)
{
	int			i = 0;
	PTASKDEF	td = NULL;
	
	for (i = 0;i < MAX_TASKS;i++) {
		td = &taskDefs[i];
		
		if (td->ID == taskID) {
			td->startTime = _realTimeClock;
			td->scheduledTime = _getScheduledTime(td->startTime, td->delay);
			td->isScheduled = 1;
			td->pParameter = p;
			break;
		}
	}
}

/******************************************************************************
**
** Name: unscheduleTask()
**
** Description: Un-schedules a task that has previously been scheduled, e.g. this
** will cancel the scheduled run.
**
** Parameters:	
** uint16_t		taskID		The unique ID for the task
**
** Returns:		void 
**
******************************************************************************/
void unscheduleTask(uint16_t taskID)
{
	int			i = 0;
	PTASKDEF	td = NULL;
	
	for (i = 0;i < MAX_TASKS;i++) {
		td = &taskDefs[i];
		
		if (td->ID == taskID) {
			td->startTime = 0;
			td->scheduledTime = 0;
			td->isScheduled = 0;
			td->pParameter = NULL;
			break;
		}
	}
}

/******************************************************************************
**
** Name: schedule()
**
** Description: The main scheduler loop, this will loop forever waiting for 
** tasks to be scheduled. This must be the last function called from main().
**
** Parameters:	N/A
**
** Returns:		it doesn't 
**
******************************************************************************/
void schedule()
{
	int			i = 0;
	PTASKDEF	td = NULL;
	
	/*
	** Scheduler loop, run forever waiting for tasks to be
	** scheduled...
	*/
	while (1) {
		td = &taskDefs[i];
		
		if (td->isAllocated) {
			if (td->isScheduled && (_realTimeClock >= td->scheduledTime)) {
				/*
				** Mark the task as un-scheduled, so by default the
				** task will not run again automatically. If the task
				** reschedules itself, this flag will be reset to 1...
				*/
				td->isScheduled = 0;
				
				signalCPUTrackingStart();

				/*
				** Run the task...
				*/
				td->run(td->pParameter);

				signalCPUTrackingEnd();
			}
		}
		
		i++;
		
		if (i == MAX_TASKS) {
			i = 0;
		}
	}
}
