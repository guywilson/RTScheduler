#include <stddef.h>
#include <stdint.h>
#include <scheduler.h>
#include <variant.h>

#include "rtc_samd21.h"
#include "heartbeat.h"
#include "led_utils.h"
#include "taskdef.h"

void HeartbeatTask(PTASKPARM p)
{
    static uint8_t on = 0;

    if (on) {
        turnOff();
        on = 0;
        scheduleTaskOnce(TASK_HEARTBEAT, rtc_val_ms(950), NULL);
    }
    else {
        turnOn();
        on = 1;
        scheduleTaskOnce(TASK_HEARTBEAT, rtc_val_ms(50), NULL);
    }
}
