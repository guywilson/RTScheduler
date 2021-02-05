#include <scheduler.h>

#ifndef _INCL_RTC_SAMD21
#define _INCL_RTC_SAMD21

/*
 * Set the RTC clock frequency to 1KHz (1ms tick). This is a
 * good frequency for the real-time scheduler...
 */
#define RTC_CLOCK_FREQ					1000

/*
 * Convenience macros for scheduler time periods...
 */
#define rtc_val_ms(time_in_ms)				((time_in_ms) * (RTC_CLOCK_FREQ / 1000))

#define rtc_val_sec(time_in_sec)			rtc_val_ms(time_in_sec * 1000)
#define rtc_val_min(time_in_min)			rtc_val_sec(time_in_min * 60)
#define rtc_val_hr(time_in_hr)				rtc_val_min(time_in_hr * 60)

#define RTC_ONE_SECOND						rtc_val_ms(1000)
#define RTC_ONE_MINUTE						rtc_val_ms(60000)
#define RTC_ONE_HOUR						rtc_val_ms(3600000)

int sysTickHook();

#endif
