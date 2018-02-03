#include <IRremoteESP8266.h>
//#include <IRremoteInt.h>
#include "ir.h"
#include "motor.h"
#include "main.h"
#include "debug.h"

#define IR_DBG
#ifdef IR_DBG
  #define IR_DbgMsg DbgMsg
#else
  #define IR_DbgMsg
#endif


//samsung remote repeat time: 108 ms

IRrecv IRRecv(PIN_D5);
decode_results IRRes;

//local variables
volatile unsigned char FlagBtnRepeat;
unsigned char btnPress = 0;
unsigned char firstpress = 1;
unsigned char btnDown = 0;
BtnSend res;

void InitIR()
{
  btnPress = 0;
  firstpress = 1;
  btnDown = 0;
  
	IRRecv.enableIRIn(); // Start the receiver
}



BtnSend IRTask(void)
{
  res.BtnState = BTN_NOACTION;

  //has a button been received
  if(IRRecv.decode(&IRRes))
  {
    IR_DbgMsg("ir:code: %d\n", IRRes.value);
    IRRecv.resume();
    btnPress = 1;
  }
	
	//button repaeat time
	if (FlagBtnRepeat)
	{
		//has button been pressed
		if(btnPress)
		{
			res.BtnId = IRRes.value;

			btnDown = 1;
			btnPress = 0;

			if (firstpress)
			{
				IR_DbgMsg("ir:PRESS: %d\n", res.BtnId);
				res.BtnState = BTN_PRESS;
				firstpress = 0;
			}
			else
			{
				IR_DbgMsg("ir:REPEAT: %d\n", res.BtnId);
				res.BtnState = BTN_REPEAT;
			}
		}
		//has button been pressed previously
		else if (btnDown == 1)
		{
			DbgMsg("ir:RELEASE: %d\n", res.BtnId);
			firstpress = 1;
			btnDown = 0;
			res.BtnState = BTN_RELEASE;
		}

		FlagBtnRepeat = 0;
	}

	return res;
}
