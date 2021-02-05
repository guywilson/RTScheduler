#include <scheduler.h>

#include "rtc_samd21.h"

int sysTickHook()
{
    _rtcISR();

    return 0;
}
