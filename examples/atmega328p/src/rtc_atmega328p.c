#include <stdint.h>
#include <avr/interrupt.h>
#include <scheduler.h>

#include "rtc_atmega328p.h"

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
}

ISR(TIMER1_COMPA_vect, ISR_BLOCK)
{
	_rtcISR();
}
