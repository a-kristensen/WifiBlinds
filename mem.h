#ifndef _MEM_h
#define _MEM_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


#define ADDR_CFG 0
#define ADDR_SCHEDULE_ENBABLED ADDR_CFG + 0

#define ADDR_TIMERULES 0x10


#endif

