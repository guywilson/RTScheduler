#include <stdint.h>
#include <samd.h>

#ifndef _INCL_LEDUTILS
#define _INCL_LEDUTILS

void		setupLEDPin(void);
void		turnOn(uint32_t LED_ID);
void		turnOff(uint32_t LED_ID);
void 		toggleLED(uint32_t LED_ID);

#endif
