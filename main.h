#ifndef _MAIN_h
#define _MAIN_h

#include "arduino.h"


//global debug on/off
#define USE_DEBUG
#ifdef USE_DEBUG
  #define DbgMsg Serial.printf
#else
  #define DbgMsg
#endif

#define SW_VERSION 101
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__


//pinouts for NodeMCU
#define MYPIN_D0   16 //LED
#define MYPIN_D1   5
#define MYPIN_D2   4
#define MYPIN_D3   0
#define MYPIN_D4   2
#define MYPIN_D5   14
#define MYPIN_D6   12
#define MYPIN_D7   13
#define MYPIN_D8   15
#define MYPIN_D9   3
#define MYPIN_D10  1
#define MYPIN_SDA  4
#define MYPIN_SCL  5
#define MYPIN_SS   15
#define MYPIN_MOSI 13
#define MYPIN_MISO 12
#define MYPIN_SCK  14
#define MYPIN_A0   17






#endif

