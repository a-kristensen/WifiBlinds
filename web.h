#ifndef _WEB_h
#define _WEB_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif

void InitWeb();
void WebTask(void);

#endif



