#include "motor.h"
#include "main.h"
#include "debug.h"

//#define MOTOR_DBG
#ifdef MOTOR_DBG
  #define MOTOR_DbgMsg DbgMsg
#else
  #define MOTOR_DbgMsg
#endif

volatile signed long MotPosTck = 0;
volatile unsigned char MotorSt = MOTOR_STOP;

void InitMotor(void)
{
	pinMode(PIN_IA, OUTPUT);
	pinMode(PIN_IB, OUTPUT);
}

void Run_Open(void)
{
	if (MotPosTck < MAX_TCK_OPEN)
	{
		MOTOR_DbgMsg("motor:RUN_OPEN\n");
		digitalWrite(PIN_IA, LOW);
		MotorSt = MOTOR_RUN_OPEN;
		digitalWrite(PIN_IB, HIGH);
	}
}

void Run_Close(void)
{
	if (MotPosTck > 0)
	{
		MOTOR_DbgMsg("motor:RUN_CLOSE\n");
		digitalWrite(PIN_IA, HIGH);
		MotorSt = MOTOR_RUN_CLOSE;
		digitalWrite(PIN_IB, LOW);
	}
}

void MotorStop(void)
{
	MOTOR_DbgMsg("motor:STOP\n");
	MotorSt = MOTOR_STOP;
	digitalWrite(PIN_IA, LOW);
	digitalWrite(PIN_IB, LOW);
}
