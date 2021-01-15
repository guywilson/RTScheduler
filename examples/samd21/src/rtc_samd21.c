#include <stdint.h>
#include <tc.h>
#include <scheduler.h>

#include "rtc_samd21.h"

struct tc_module			tc_instance;

void setupRTC()
{
	struct tc_config		config_tc;
	
	tc_get_config_defaults(&config_tc);
	
	config_tc.counter_size = TC_COUNTER_SIZE_8BIT;
	config_tc.clock_source = GCLK_GENERATOR_0;
	config_tc.clock_prescaler = TC_CLOCK_PRESCALER_DIV4;
	config_tc.counter_8_bit.period = 46 ; 

	tc_init(&tc_instance, CONF_TC_MODULE, &config_tc);
	tc_enable(&tc_instance);
}

// ISR(TIMER1_COMPA_vect, ISR_BLOCK)
// {
// 	_rtcISR();
// }
