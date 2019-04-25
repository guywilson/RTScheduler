#include <stdint.h>
#include <avr/interrupt.h>

#include "rtc_atmega328p.h"

volatile uint32_t 			_realTimeClock;
static volatile uint16_t	_tickCount = 0;

static void (* _tickTask)();

void nullTick()
{
	// Do nothing...
}

void setupRTC()
{
    // Clear registers
    TCCR1A = 0x00;
    TCCR1B = 0x00;
    
    TCNT1  = 0;
    
    /*
    ** Timer value = (clock_speed / (interrupt_freq * pre-scaler)) - 1
    ** Timer value = (16,000,000 / (1000 * 8)) - 1
    */
    OCR1A = (F_CPU / __CLOCK_DIVISOR) - 1;

    // turn on CTC mode
    TCCR1B |= (1 << WGM12);
    
    // Set CS11 bit for 8 prescaler
    TCCR1B |= (1 << CS11);  
    
    // enable timer compare interrupt
    TIMSK1 |= (1 << OCIE1A);

    // Register the nullTick() function...
    registerTickTask(&nullTick);

    _realTimeClock = 0L;

    initialiseCPUTracking();
}

uint32_t getCurrentTime()
{
	return _realTimeClock;
}

void registerTickTask(void (* tickTask)())
{
	_tickTask = tickTask;
}

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
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

	signalCPUTrackingStart();

	/*
	 * Run the tick task, defaults to the nullTick() function.
	 *
	 * This must be a very fast operation, as it is outside of
	 * the scheduler's control. Also, there can be only 1 tick task...
	 */
	_tickTask();

	signalCPUTrackingEnd();
}
