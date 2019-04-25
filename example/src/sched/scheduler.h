#include <stdint.h>

#ifndef _INCL_SCHEDULER
#define _INCL_SCHEDULER

#define MAX_TASKS           	16

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


/******************************************************************************
**
** Used for tracking CPU usage (e.g. how much time is spent running tasks). If
** TRACK_CPU_PCT is defined, then the example below will toggle the on-board
** LED on an Arduino.
**
******************************************************************************/
#ifndef TRACK_CPU_PCT
#define signalCPUTrackingStart()		// Do nothing
#define signalCPUTrackingEnd()			// Do nothing
#else
#include <avr/io.h>
#define signalCPUTrackingStart()		PORTB |= _BV(PORTB5)
#define signalCPUTrackingEnd()			PORTB &= ~(_BV(PORTB5))
#endif


/******************************************************************************
**
** The real-time clock that drives the scheduler. Typically this
** will be incremented by a timer interrupt
**
******************************************************************************/
extern volatile uint32_t 		_realTimeClock;


/******************************************************************************
**
** The scheduler API
**
******************************************************************************/
void		initScheduler();

void		registerTask(uint16_t taskID, void (* run)(PTASKPARM));
void		deregisterTask(uint16_t taskID);

void		scheduleTask(uint16_t taskID, timer_t time, PTASKPARM p);
void		rescheduleTask(uint16_t taskID, PTASKPARM p);
void		unscheduleTask(uint16_t taskID);

void		schedule();

#endif
