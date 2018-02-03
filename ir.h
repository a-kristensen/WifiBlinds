#ifndef _IR_h
#define _IR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

typedef struct {
	unsigned long BtnId;
	unsigned char BtnState;
} BtnSend;

enum ButtonAction {
	BTN_NOACTION = 0,
	BTN_PRESS = 1,
	BTN_REPEAT = 2,
	BTN_RELEASE = 3
};

#define BTN_RED     0xDAEA83EC
#define BTN_GREEN   0x2BAFCEEC
#define BTN_YELLOW  0xB5210DA6
#define BTN_BLUE    0x71A1FE88


BtnSend IRTask(void);
void InitIR();

extern volatile unsigned char FlagBtnRepeat;

#endif

