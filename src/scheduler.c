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
#define TRACK_CPU_PCT

/******************************************************************************
**
** The TASKDESC struct.
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
	uint8_t			priority;		// Task priority 0 (highest) to 5 (lowest)
	uint8_t			type;			// Task type - Periodic, On-demand
	uint8_t			isScheduled;	// Is this task scheduled
	uint8_t			isAllocated;	// Is this allocated to a task
	PTASKPARM		pParameter;		// The parameters to the task
	
	void (* run)(PTASKPARM);		// Pointer to the task function to run
									// Must be of the form void task(PTASKPARM p);
	void * 			next;
	void *			prev;
}
TASKDESC;

typedef TASKDESC *	PTASKDESC;

/******************************************************************************
**
** Name: _nullTask()
**
** Description: Null task assigned to TASKDESC entries that are unused.
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
** Name: _nullTickTask()
**
** Description: Null tick task.
**
** Parameters:	N/A
**
** Returns:		void 
**
******************************************************************************/
void _nullTickTask()
{
	// Do nothing...
}

TASKDESC					taskDescs[MAX_TASKS];	// Array of tasks for the scheduler
int							taskCount = 0;			// Number of tasks registered

PTASKDESC					head = NULL;			// Pointer to the head of the scheduled task queue

volatile uint32_t 			_realTimeClock = 0;		// The real time clock counter
volatile uint16_t			_tickCount = 0;			// Num ticks between rtc counts

volatile uint32_t			_busyCount = 0;
volatile uint32_t			_idleCount = 0;

// The RTC tick task...
void 						(* _tickTask)() = &_nullTickTask;

#ifdef TRACK_CPU_PCT
#define signalBusy()		_busyCount++
#define signalIdle()		_idleCount++
#else
#define signalBusy()		// Do nothing
#define signalIdle()		// Do nothing
#endif

/******************************************************************************
**
** Name: _rtcISR()
**
** Description: The RTC interrupt handler.
**
** Parameters:	N/A
**
** Returns:		void 
**
******************************************************************************/
void _rtcISR()
{
	_tickCount++;

	if (_tickCount == RTC_INTERRUPT_PRESCALER) {
	    /*
	    ** The RTC is incremented every 1 ms,
		** it is used to drive the real time clock
		** for the scheduler...
	    */
		_realTimeClock++;

		_tickCount = 0;
	}

	/*
	 * Run the tick task, defaults to the nullTick() function.
	 *
	 * This must be a very fast operation, as it is outside of
	 * the scheduler's control. Also, there can be only 1 tick task...
	 */
	signalBusy();
	_tickTask();
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
static timer_t _getScheduledTime(timer_t startTime, timer_t requestedDelay)
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
** Name: _findTaskByID()
**
** Description: Returns a pointer to the task identifed by the supplied ID
**
** Parameters:
**				uint16_t	taskID			The taskID to find
**
** Returns: 
**				PTASKDESC	The pointer to the task, can be NULL
**
******************************************************************************/
static PTASKDESC _findTaskByID(uint16_t taskID)
{
	int			i = 0;
	PTASKDESC	td = NULL;

	for (i = 0;i < MAX_TASKS;i++) {
		td = &taskDescs[i];
		
		if (td->ID == taskID) {
			break;
		}
	}

	return td;
}

/******************************************************************************
**
** Name: _scheduleTask()
**
** Description: Schedules the task to run after the specified delay. A task
** must be registered using registerTask() before it can be scheduled.
**
** Parameters:	
** uint16_t		taskID		The unique ID for the task
** timer_t		time		Number of ms in the future for the task to run
** uint8_t		priority	The priority of the task
** PTASKPARM	p			Pointer to the task parameters, can be NULL
**
** Returns:		A pointer to the task definition 
**
******************************************************************************/
static PTASKDESC _scheduleTask(PTASKDESC td, timer_t time, uint8_t priority, PTASKPARM p)
{
	PTASKDESC	current = NULL;

	if (td != NULL) {
		td->startTime = _realTimeClock;
		td->delay = time;
		td->scheduledTime = _getScheduledTime(td->startTime, td->delay);
		td->priority = priority;
		td->isScheduled = 1;
		td->pParameter = p;

		if (head != NULL) {
			current = head;

			/*
			** Iterate to the end of the list...
			*/
			while (current != NULL) {
				if (current->scheduledTime > td->scheduledTime) {
					/*
					** Insert our task in here...
					*/
					if (current->prev != NULL) {
						td->prev = current->prev;
						td->next = current;
						((PTASKDESC)(current->prev))->next = td;
						break;
					}
				}
				else if (current->scheduledTime == td->scheduledTime) {
					/*
					** Now it's a question of priority...
					*/
					if (current->priority >= td->priority) {
						/*
						** Insert our task in here...
						*/
						if (current->prev != NULL) {
							td->prev = current->prev;
							td->next = current;
							((PTASKDESC)(current->prev))->next = td;
							break;
						}
					}
				}

				current = current->next;
			}
		}
		else {
			/*
			** This must be the first task...
			*/
			head = td;

			/*
			** Point its next & prev ptrs to NULL...
			*/
			head->next = NULL;
			head->prev = NULL;
		}
	}

	return td;
}

/******************************************************************************
**
** Public API functions
**
******************************************************************************/

/******************************************************************************
**
** Name: getCPURatio()
**
** Description: Returns the busy & idle counts.
**              In order to calculate the ratio of busy/idle, the caller must
**				calculate:
**
**				busy/idle ratio = busyCount / (idleCount - busyCount)
**
** Parameters:	uint32_t *		idleCount	A pointer to a 32-bit unsigned int
**				uint32_t *		busyCount	A pointer to a 32-bit unsigned int
**
** Returns:		void 
**
******************************************************************************/
void getCPURatio(uint32_t * idleCount, uint32_t * busyCount)
{
	*idleCount = _idleCount;
	*busyCount = _busyCount;

	// Reset counters...
	_busyCount = 0;
	_idleCount = 0;
}

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
	PTASKDESC	td = NULL;
	
	taskCount = 0;
	
	for (i = 0;i < MAX_TASKS;i++) {
		td = &taskDescs[i];
		
		td->ID				= 0;
		td->startTime		= 0;
		td->scheduledTime	= 0;
		td->delay			= 0;
		td->priority		= TASK_PRIORITY_NORMAL;
		td->type			= TASK_TYPE_ON_DEMAND;
		td->isScheduled		= 0;
		td->isAllocated		= 0;
		td->pParameter		= NULL;
		td->run				= &_nullTask;

		td->next			= NULL;
		td->prev			= NULL;
	}
}

/******************************************************************************
**
** Name: registerTickTask()
**
** Description: Registers the scheduler tick task. This tick task is called 
**				from the RTC ISR.
**
** Parameters:	void 	(* tickTask)	Pointer to the tick task function
**
** Returns:		void 
**
******************************************************************************/
void registerTickTask(void (* tickTask)())
{
	_tickTask = tickTask;
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
	int			i;
	PTASKDESC	td = NULL;
	uint8_t		noFreeTasks = 1;

	for (i = 0;i < MAX_TASKS;i++) {
		td = &taskDescs[i];

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
	PTASKDESC	td = NULL;

	td = _findTaskByID(taskID);

	if (td != NULL) {
		td->ID				= 0;
		td->startTime		= 0;
		td->scheduledTime	= 0;
		td->delay			= 0;
		td->isScheduled		= 0;
		td->isAllocated		= 0;
		td->pParameter		= NULL;
		td->run				= &_nullTask;
		
		taskCount--;
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
** uint8_t		priority	The priority of the task
** PTASKPARM	p			Pointer to the task parameters, can be NULL
**
** Returns:		void 
**
******************************************************************************/
void scheduleTask(uint16_t taskID, timer_t time, uint8_t priority, PTASKPARM p)
{
	PTASKDESC	td = NULL;

	td = _findTaskByID(taskID);

	if (td != NULL) {
		_scheduleTask(td, time, priority, p);
	}

	td->type = TASK_TYPE_ON_DEMAND;	
}

/******************************************************************************
**
** Name: scheduleTaskPeriodic()
**
** Description: Schedules the task to run continuosly after the specified 
** delay. A task must be registered using registerTask() before it can be 
** scheduled.
**
** Parameters:	
** uint16_t		taskID		The unique ID for the task
** timer_t		time		Number of ms in the future for the task to run
** uint8_t		priority	The priority of the task
** PTASKPARM	p			Pointer to the task parameters, can be NULL
**
** Returns:		void 
**
******************************************************************************/
void scheduleTaskPeriodic(uint16_t taskID, timer_t time, uint8_t priority, PTASKPARM p)
{
	PTASKDESC	td = NULL;

	td = _findTaskByID(taskID);

	if (td != NULL) {
		_scheduleTask(td, time, priority, p);
	}

	td->type = TASK_TYPE_PERIODIC;	
}

/******************************************************************************
**
** Name: rescheduleTask()
**
** Description: Reschedules the task to run again after the same delay as
** specified in the scheduleTask() call. Useful for calling at the end of
** the task itself to force it to run again (after the specified delay). If
** you have a task that always runs at a regular time, it is recommended you
** call scheduleTaskPeriodic() which will ensure the task is run at the 
** specified period.
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
	PTASKDESC	td = NULL;

	td = _findTaskByID(taskID);

	if (td != NULL) {
		_scheduleTask(td, td->delay, td->priority, p);
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
	PTASKDESC	td = NULL;

	td = _findTaskByID(taskID);

	if (td != NULL) {
		td->startTime = 0;
		td->scheduledTime = 0;
		td->isScheduled = 0;
		td->pParameter = NULL;

		/*
		** Unlink the task from the scheduled queue...
		*/
		if (td->prev != NULL) {
			((PTASKDESC)(td->prev))->next = td->next;
		}
		if (td->next != NULL) {
			((PTASKDESC)(td->next))->prev = td->prev;
		}

		td->next = NULL;
		td->prev = NULL;
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
	PTASKDESC	td = NULL;
	
	/*
	** Scheduler loop, run forever waiting for tasks to be
	** scheduled...
	*/
	while (1) {
		/*
		** Get the task at the head of the scheduled queue...
		*/
		td = head;
		
		signalIdle();

		if (td != NULL) {
			if (_realTimeClock >= td->scheduledTime) {
				/*
				** Mark the task as un-scheduled, so by default the
				** task will not run again automatically. If the task
				** reschedules itself, this flag will be reset to 1...
				*/
				td->isScheduled = 0;

				/*
				** Unlink the task...
				*/
				if (td->next != NULL) {
					((PTASKDESC)(td->next))->prev = NULL;
					head = (PTASKDESC)(td->next);
					td->next = NULL;
				}
				
				signalBusy();

				/*
				** Run the task...
				*/
				td->run(td->pParameter);

				/*
				** If the task type is periodic, reschedule the task...
				*/
				if (td->type == TASK_TYPE_PERIODIC && !td->isScheduled) {
					rescheduleTask(td->ID, td->pParameter);
				}
			}
		}
	}
}
