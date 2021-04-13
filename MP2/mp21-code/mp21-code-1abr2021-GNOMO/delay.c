#include "delay.h"

#ifdef DELAY
int delay = DELAY; // in milisec
#else
int delay = 100;
#endif