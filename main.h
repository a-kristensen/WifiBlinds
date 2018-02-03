// main.h
#ifndef _MAIN_h
#define _MAIN_h

#define SW_VERSION 101

extern unsigned long UserPosTck;
extern unsigned char validTime;


void CmdClose();
void CmdOpen();
void CmdResetPos();

//pinouts for NodeMCU
#define PIN_D0   16 //LED
#define PIN_D1   5
#define PIN_D2   4
#define PIN_D3   0
#define PIN_D4   2
#define PIN_D5   14
#define PIN_D6   12
#define PIN_D7   13
#define PIN_D8   15
#define PIN_D9   3
#define PIN_D10  1
#define PIN_SDA  4 
#define PIN_SCL  5 
#define PIN_SS   15 
#define PIN_MOSI 13 
#define PIN_MISO 12 
#define PIN_SCK  14 
#define PIN_A0   17 


enum CurtainState {
	CLOSED_POS,		  //in closed postition
	USER_POS,		    //in user position
	USER_RUN_OPEN,	//running open, while holding button
	USER_RUN_CLOSE,	//running close, while holding button
	RUN_TO_CLOSE,	  //running to closed position
	RUN_TO_USERPOS,	//running to user position
	MASTER_TOGGLE,  //master toggle, disallowing interruptions
};

#endif

