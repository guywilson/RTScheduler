#include <scheduler.h>

#include "rtc_samd21.h"

int sysTickHook()
{
    /*
    ** Call the scheduler ISR...
    */
    _rtcISR();

    return 0;
}
