#include "schedule.h"
#include "datetime.h"
#include "main.h"
#include "debug.h"

#define SCHEDULE_DBG
#ifdef SCHEDULE_DBG
  #define SCHEDULE_DbgMsg DbgMsg
#else
  #define SCHEDULE_DbgMsg
#endif

unsigned char RulesActive;
unsigned char noOfRules;
unsigned char ScheduleMinFlag;

DateTime currT;
rule_item* ptrRules;

void InitSchedule(void)
{
  RulesActive = 1;
  ptrRules = new rule_item[NO_OF_RULES];

  //wd, h, m, cmd
  for(unsigned char i = 0; i<7; i++)
  {
    ptrRules[i*2] = (rule_item){i+1, 8, 0, SCHEDULE_OPEN, 1};
    ptrRules[(i*2)+1] = {i+1, 19, 0, SCHEDULE_CLOSE, 1};
  }
}

void ScheduleTask(void)
{

  
  if(RulesActive)
  {
    if(validTime)
    {
      if(ScheduleMinFlag)
      {
        currT = GetTime();        
        for(int i=0; i<NO_OF_RULES; i++)
        {
          //are current rule enabled?
          if(ptrRules[i].ena == 1)
          {
            if(currT.wday == ptrRules[i].wd)
            {
              if(currT.hour == ptrRules[i].h)
              {
                if(currT.min == ptrRules[i].m)
                {
                  if(ptrRules[i].cmd == 1)
                  {
                    CmdClose();
                    SCHEDULE_DbgMsg("schedule: cmd_close\n");
                  }
                  else if(ptrRules[i].cmd == 2)
                  {
                    CmdOpen();
                    SCHEDULE_DbgMsg("schedule: cmd_open\n");
                  }
                }
              }
            }
          }
        }
        //all rules carried out, wait for next minute
        ScheduleMinFlag = 0;
      }
    }
  }
}

