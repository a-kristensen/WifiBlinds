#ifndef _DEBUG_h
#define _DEBUG_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif

//global debug on/off
#define USE_DEBUG
#ifdef USE_DEBUG
  #define DbgMsg Serial.printf
#else
  #define DbgMsg
#endif


#endif



