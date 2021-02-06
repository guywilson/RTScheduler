#include <stddef.h>

#include <Arduino.h>
#include <samd.h>
#include <variant.h>

#include <scheduler.h>
#include <schederr.h>

#include "taskdef.h"

#include "rtc_samd21.h"
#include "heartbeat.h"
#include "led_utils.h"

void initialiseSAMD21()
{
	/*
	** Set SysTick to 100us...
	*/
	if (SysTick_Config(100ul)) {
		while (1) ;
	}

	NVIC_SetPriority (SysTick_IRQn,  (1 << __NVIC_PRIO_BITS) - 2);  /* set Priority for Systick Interrupt (2nd lowest) */

	// Clock SERCOM for Serial
	PM->APBCMASK.reg |= 
			(PM_APBCMASK_SERCOM0 | 
			PM_APBCMASK_SERCOM1 | 
			PM_APBCMASK_SERCOM2 | 
			PM_APBCMASK_SERCOM3 | 
			PM_APBCMASK_SERCOM4 | 
			PM_APBCMASK_SERCOM5);

	// Clock TC/TCC for Pulse and Analog
	PM->APBCMASK.reg |= 
			(PM_APBCMASK_TCC0 | 
			PM_APBCMASK_TCC1 | 
			PM_APBCMASK_TCC2 | 
			PM_APBCMASK_TC3 | 
			PM_APBCMASK_TC4 | 
			PM_APBCMASK_TC5);

	// Clock ADC/DAC for Analog
	PM->APBCMASK.reg |= PM_APBCMASK_ADC | PM_APBCMASK_DAC ;

	// Defining VERY_LOW_POWER breaks Arduino APIs since all pins are considered INPUT at startup
	// However, it really lowers the power consumption by a factor of 20 in low power mode (0.03mA vs 0.6mA)
#ifndef VERY_LOW_POWER
	// Setup all pins (digital and analog) in INPUT mode (default is nothing)
	for (uint32_t ul = 0; ul < NUM_DIGITAL_PINS; ul++) {
		pinMode( ul, INPUT ) ;
	}
#endif

	// Initialize Analog Controller
	// Setting clock
	while(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);

	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID( GCM_ADC ) | // Generic Clock ADC
						GCLK_CLKCTRL_GEN_GCLK0     | // Generic Clock Generator 0 is source
						GCLK_CLKCTRL_CLKEN ;

	while(ADC->STATUS.bit.SYNCBUSY == 1);          // Wait for synchronization of registers between the clock domains

	ADC->CTRLB.reg = ADC_CTRLB_PRESCALER_DIV512 |    // Divide Clock by 512.
					ADC_CTRLB_RESSEL_10BIT;         // 10 bits resolution as default

	ADC->SAMPCTRL.reg = 0x3f;                        // Set max Sampling Time Length

	while(ADC->STATUS.bit.SYNCBUSY == 1);          // Wait for synchronization of registers between the clock domains

	ADC->INPUTCTRL.reg = ADC_INPUTCTRL_MUXNEG_GND;   // No Negative input (Internal Ground)

	// Averaging (see datasheet table in AVGCTRL register description)
	ADC->AVGCTRL.reg = ADC_AVGCTRL_SAMPLENUM_1 |    // 1 sample only (no oversampling nor averaging)
						ADC_AVGCTRL_ADJRES(0x0ul);   // Adjusting result by 0

	analogReference(AR_DEFAULT); // Analog Reference is AREF pin (3.3v)

	// Initialize DAC
	// Setting clock
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
	
	GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID( GCM_DAC ) | // Generic Clock ADC
						GCLK_CLKCTRL_GEN_GCLK0     | // Generic Clock Generator 0 is source
						GCLK_CLKCTRL_CLKEN ;

	while (DAC->STATUS.bit.SYNCBUSY == 1); // Wait for synchronization of registers between the clock domains
	
	DAC->CTRLB.reg = DAC_CTRLB_REFSEL_AVCC | // Using the 3.3V reference
					DAC_CTRLB_EOEN ;        // External Output Enable (Vout)
}

int main(void)
{
 	initialiseSAMD21();

	setupLEDPin();
	
	initScheduler(16);
	
	registerTask(TASK_HEARTBEAT, &HeartbeatTask);

	scheduleTaskOnce(
			TASK_HEARTBEAT,
			rtc_val_ms(950),
			NULL);

	/*
	** Start the scheduler...
	*/
	schedule();
	
	return -1;
}
