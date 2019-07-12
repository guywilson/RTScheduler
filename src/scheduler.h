#include <stdint.h>

#ifndef _INCL_SCHEDULER
#define _INCL_SCHEDULER

#define MAX_TASKS           	32

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

/*
** Passed as the time parameter to scheduleTask.
*/
#define RUN_NOW                 0

/******************************************************************************
**
** Task priorities
**
******************************************************************************/
#define TASK_PRIORITY_HIGHEST   0
#define TASK_PRIORITY_HIGH      1
#define TASK_PRIORITY_MED_HIGH  2
#define TASK_PRIORITY_NORMAL    3
#define TASK_PRIORITY_LOW       4
#define TASK_PRIORITY_LOWEST    5

/******************************************************************************
**
** Task types
**
******************************************************************************/
#define TASK_TYPE_PERIODIC      0x01
#define TASK_TYPE_ON_DEMAND     0x02

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
** The scheduler API
**
******************************************************************************/
void		initScheduler();

void        registerTickTask(void (* tickTask)());

void		registerTask(uint16_t taskID, void (* run)(PTASKPARM));
void		deregisterTask(uint16_t taskID);

void        scheduleTask(uint16_t taskID, timer_t time, uint8_t priority, PTASKPARM p);
void        scheduleTaskPeriodic(uint16_t taskID, timer_t time, uint8_t priority, PTASKPARM p);
void		rescheduleTask(uint16_t taskID, PTASKPARM p);
void		unscheduleTask(uint16_t taskID);

void		schedule();

#endif
