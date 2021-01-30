#include <stdint.h>
#include <samd.h>

#ifndef _INCL_LEDUTILS
#define _INCL_LEDUTILS

void		setupLEDPin(void);
void		turnOn();
void		turnOff();
void 		toggleLED();

#endif
