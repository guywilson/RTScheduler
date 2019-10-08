#include <stdint.h>

#ifndef _INCL_SCHEDULER
#define _INCL_SCHEDULER

#define DEFAULT_MAX_TASKS       16

typedef void *					PTASKPARM;

/*
** Define timer type for bit-depth of timer, change this
** if your timer is 64-bit for example...
*/
#ifndef ARCH_SIZE
#define ARCH_SIZE		16
#endif

#if ARCH_SIZE <= 16
typedef uint32_t				timer_t;
#else
typedef uint64_t				timer_t;
#endif

#define MAX_TIMER_VALUE			~((timer_t)0)

#ifdef UNIT_TEST_MODE
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

PTASKDESC 	getRegisteredTasks();
int 		isLastTask(PTASKDESC td);
#endif

/*
** Passed as the time parameter to scheduleTask.
*/
#define RUN_NOW                 0

/*
 * If, for example, you want a faster interrupt frequency for the
 * RTC tick task, set the prescaler here. If you want the interrupt
 * frequency and clock frequency to be the same, simply set this to 1...
 */
#define RTC_INTERRUPT_PRESCALER			4

/******************************************************************************
**
** The real-time clock interrupt service routing
**
******************************************************************************/
void        _rtcISR();

/******************************************************************************
**
** Get the last recorded busy/idle CPU counts
**
******************************************************************************/
void getCPURatio(uint32_t * idleCount, uint32_t * busyCount);

/******************************************************************************
**
** Get the scheduler version string and build date
**
******************************************************************************/
const char * getSchedulerVersion();
const char * getSchedulerBuildDate();

/******************************************************************************
**
** The scheduler API
**
******************************************************************************/
void 		initScheduler(int size);

void        registerTickTask(void (* tickTask)());

void		registerTask(uint16_t taskID, void (* run)(PTASKPARM));
void		deregisterTask(uint16_t taskID);

void        scheduleTask(uint16_t taskID, timer_t time, PTASKPARM p);
void		rescheduleTask(uint16_t taskID, PTASKPARM p);
void		unscheduleTask(uint16_t taskID);

void		schedule();

#endif
