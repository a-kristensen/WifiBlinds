#ifndef _MOTOR_h
#define _MOTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

//10 rounds, 2000ms/rnd
#define MAX_TCK_OPEN	10000
#define PIN_IA	PIN_D1
#define PIN_IB	PIN_D2

enum MotorState {
	MOTOR_STOP,
	MOTOR_RUN_OPEN,
	MOTOR_RUN_CLOSE
};

void InitMotor(void);
void Run_Open(void);
void Run_Close(void);
void MotorStop(void);

extern volatile signed long MotPosTck;
extern volatile unsigned char MotorSt;

#endif

