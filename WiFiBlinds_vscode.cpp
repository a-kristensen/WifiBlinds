//#include <Ticker.h>
#include <vector>
#include "main.h"
#include "blinds.h"
#include "ir.h"
#include "datetime.h"
#include "web.h"
#include "schedule.h"
#include "mqtt.h"
#include "param.h"
#include "wifi.h"
#include "multi-ctrl.h"


#define MAIN_DBG
#ifdef MAIN_DBG
  #define MAIN_DbgMsg DbgMsg
#else
  #define MAIN_DbgMsg
#endif

//Ticker tck;

//class_blinds*   ptr_blinds    = new class_blinds();     //singleton, so no need anymore

class_param*    ptr_param     = new class_param();
class_wifi*     ptr_wifi      = new class_wifi(ptr_param);
class_datetime* ptr_datetime  = new class_datetime();

// all users of blinds below
class_schedule* ptr_sched     = new class_schedule(ptr_datetime);
class_mqtt*     ptr_mqtt      = new class_mqtt();
class_ir*       ptr_ir        = new class_ir();
class_web*      ptr_web       = new class_web(ptr_datetime, ptr_param);
class_multi*    ptr_multi     = new class_multi();


void ISRTmr1()
{
  static unsigned int TmrCnt = 0;
  static unsigned char CntMins = 0;
  
  //milliseconds invoke --------
  TaskScheduler::runAllTimer1ms();

  if(TmrCnt++ >= 1000)
  {
    TmrCnt = 0;

    //seconds invoke -----------
    TaskScheduler::runAllTimer1sec();

    if(CntMins++ > 60)
    {
      CntMins = 0;

      //minutes invoke ---------
      TaskScheduler::runAllTimer1min();
    }
  }
}

void setup()
{
  Serial.begin(115200);
  MAIN_DbgMsg("\n\n-------- WifiBlinds Started ---------\n");
  MAIN_DbgMsg("--- Build: " BUILD_DATE " - " BUILD_TIME " ---\n");
  MAIN_DbgMsg("------- Software version: %u.%u -------\n\n", SW_VERSION/100, SW_VERSION%100);

  TaskScheduler::initTasks();

  //tck.attach_ms(1, ISRTmr1);

  //interrupt setup
  timer1_attachInterrupt(ISRTmr1);
  timer1_isr_init();
  timer1_write(5000);
  //TIM_DIV16:  5MHz (5 ticks/us - 1677721.4 us max)
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
}


void loop()
{
  TaskScheduler::runSchedOnce();
}
