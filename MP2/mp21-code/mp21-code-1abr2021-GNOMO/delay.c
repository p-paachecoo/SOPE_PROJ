#include "delay.h"

#ifdef DELAY
int delay = DELAY; // in milisec
#else
int delay = 500;
#endif