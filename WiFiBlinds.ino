 #include <stdarg.h>
#include <stdio.h>
#include "main.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <IRremoteESP8266.h>
//#include <IRremoteInt.h>
#include "arduino.h"
#include "motor.h"
#include "ir.h"
#include "datetime.h"
#include "web.h"
#include "schedule.h"
#include "debug.h"
#include "mem.h"


#define MAIN_DBG
#ifdef MAIN_DBG
  #define MAIN_DbgMsg DbgMsg
#else
  #define MAIN_DbgMsg
#endif


unsigned int Cnt1ms;
unsigned int TmrCnt = 0;
unsigned char CurtainSt = CLOSED_POS;
unsigned long UserPosTck = MAX_TCK_OPEN / 2;
unsigned char validTime = 0;
unsigned int SecCnt;


void ISRTmr1()
{
  if (MotorSt == MOTOR_RUN_OPEN)
    MotPosTck++;
  else if (MotorSt == MOTOR_RUN_CLOSE)
    MotPosTck--;

  //ir remote button repeat
  if (Cnt1ms++ >= 120)
  {
    FlagBtnRepeat = 1;
    Cnt1ms = 0;
  }

  if (TmrCnt++ >= 1000)
  {
    AddSec();
    TmrCnt = 0;
  }
}

void setup()
{
  Serial.begin(9600);
  InitMotor();
  InitIR();
  InitWeb();
  InitSchedule();

  timer1_attachInterrupt(ISRTmr1);
  timer1_isr_init();
  timer1_write(5000);
  //TIM_DIV16:  5MHz (5 ticks/us - 1677721.4 us max)
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
}

void CmdClose()
{
  if(CurtainSt == USER_POS)
  {
    CurtainSt = RUN_TO_CLOSE;
    Run_Close();
  }
}

void CmdOpen()
{
  if(CurtainSt == CLOSED_POS)
  {
    CurtainSt = RUN_TO_USERPOS;
    Run_Open();
  }
}

void CmdResetPos()
{
  MotorStop();
  CurtainSt = CLOSED_POS;
  UserPosTck = MotPosTck = 0;
}

void loop()
{
  unsigned char ToggleDirection = 0, WaitForBtnPress = 0;
  BtnSend IrCmd;

  while(1)
  {
    DTTask();
    ScheduleTask();
    IrCmd = IRTask();
    WebTask();  
    
    //master toggle button:: BLUE
    if (IrCmd.BtnState == BTN_RELEASE && IrCmd.BtnId == BTN_BLUE && CurtainSt != MASTER_TOGGLE)
    {
      if(CurtainSt == USER_POS)
      {
        Run_Close();
        MAIN_DbgMsg("MASTER_run_to_close\n");
        CurtainSt = RUN_TO_CLOSE;
      }
      else if(CurtainSt == CLOSED_POS)
      {
        Run_Open();
        MAIN_DbgMsg("MASTER_run_to_userpos\n");
        CurtainSt = RUN_TO_USERPOS;
      }
    }

    if (IrCmd.BtnState != BTN_NOACTION && IrCmd.BtnId == BTN_RED)
    {
      if(WaitForBtnPress)
      {
        if(IrCmd.BtnState != BTN_PRESS)
          IrCmd.BtnState = BTN_NOACTION;
        else
          WaitForBtnPress = 0;
      }
      
      switch (CurtainSt)
      {
      case CLOSED_POS:
        //ignore btn_press

        if (IrCmd.BtnState == BTN_REPEAT)
        {
          MAIN_DbgMsg("state_USER_RUN_OPEN\n");
          CurtainSt = USER_RUN_OPEN;
          Run_Open();
        }
        else if (IrCmd.BtnState == BTN_RELEASE)
        {
          MAIN_DbgMsg("state_RUN_TO_USERPOS\n");
          ToggleDirection = 1;
          CurtainSt = RUN_TO_USERPOS;
          Run_Open();
        }
        break;
      case USER_RUN_CLOSE:
      case USER_RUN_OPEN:
        /*if(IrCmd.BtnState == BTN_RELEASE)
        {
        MotorStop();
        UserPosTck = MotPosTck;
        CurtainSt = USER_POS;
        }
        break;*/
      case RUN_TO_CLOSE:
      case RUN_TO_USERPOS:
        if (IrCmd.BtnState == BTN_RELEASE)
        {
          MotorStop();
          UserPosTck = MotPosTck;
          CurtainSt = USER_POS;
          MAIN_DbgMsg("state_USER_POS_SET: %u\n", UserPosTck);
        }
        break;
      case USER_POS:
        //ignore btn_press

        if (IrCmd.BtnState == BTN_REPEAT)
        {        
          if (ToggleDirection)
          {
            MAIN_DbgMsg("USER_POS:Toggle_close\n");
            ToggleDirection = 0;
            Run_Close();
            CurtainSt = USER_RUN_CLOSE;
          }
          else
          {
            MAIN_DbgMsg("USER_POS:Toggle_open\n");
            ToggleDirection = 1;
            Run_Open();
            CurtainSt = USER_RUN_OPEN;
          }
        }
        else if (IrCmd.BtnState == BTN_RELEASE) //close
        {
          MAIN_DbgMsg("state_RUN_TO_CLOSE\n");
          Run_Close();
          CurtainSt = RUN_TO_CLOSE;
        }
        break;
      }
    }
    IrCmd.BtnState = BTN_NOACTION;


    //handle motor timing
    //-------------------------------------------------------------------

    //bottom stop
    if (MotPosTck < 0)
    {
      MotorStop();
      WaitForBtnPress = 1;
      CurtainSt = CLOSED_POS;
      MotPosTck = 0;
      MAIN_DbgMsg("motor_CLOSED_POS\n");
    }

    //user position
    if (MotPosTck >= UserPosTck && CurtainSt == RUN_TO_USERPOS)
    {
      MotorStop();
      CurtainSt = USER_POS;
      MAIN_DbgMsg("motor_IN_USER_POS: %u\n", UserPosTck);
    }

    //max open
    if (MotPosTck > MAX_TCK_OPEN)
    {
      MotorStop();
      WaitForBtnPress = 1;
      UserPosTck = MotPosTck = MAX_TCK_OPEN;
      CurtainSt = USER_POS;
      MAIN_DbgMsg("Max user pos\n");
    }
  }
}
