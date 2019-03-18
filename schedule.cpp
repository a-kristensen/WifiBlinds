#include "schedule.h"
#include "main.h"


#define SCHEDULE_DBG
#ifdef SCHEDULE_DBG
  #define SCHEDULE_DbgMsg(...) DbgMsg("Schedule: "); DbgMsg(__VA_ARGS__)
#else
  #define SCHEDULE_DbgMsg
#endif



class_schedule::class_schedule(class_datetime*& ref_dt)
{
  m_datetime = &ref_dt;
}

void class_schedule::TaskRun(void)
{
  if(m_MinuteFlag)
  {
    if(m_RulesActive)
    {
      if((**m_datetime).isTimeValid())
      {
        m_currT = (**m_datetime).GetTime();
        
        for(int i=0; i<m_noOfRules; i++)
        {
          if(m_currT.tm_wday == m_ptrRules[i].wd)
          {
            if(m_currT.tm_hour == m_ptrRules[i].h)
            {
              if(m_currT.tm_min == m_ptrRules[i].m)
              {
                if(m_ptrRules[i].cmd == 1)
                {
                  p_blinds->Close();
                  SCHEDULE_DbgMsg("cmd_close. Rule# %u. Time: %02u:%02u\n", i, m_currT.tm_hour, m_currT.tm_min);
                }
                else if(m_ptrRules[i].cmd == 2)
                {
                  p_blinds->Open();
                  SCHEDULE_DbgMsg("cmd_open. Rule# %u. Time: %02u:%02u\n", i, m_currT.tm_hour, m_currT.tm_min);
                }
              }
            }
          }
        }
      }
    }
    
    //toggle flag
    m_MinuteFlag = false;
  }
}

void class_schedule::TaskInit(void)
{
  m_RulesActive = true;
  m_noOfRules = 14;
  m_ptrRules = new rule_item[m_noOfRules];


  //wd, h, m, cmd
  for(uint8_t i = 0; i<7; i++)
  {
    m_ptrRules[i*2] = {(uint8_t)(i+1), 7, 0, SCHEDULE_OPEN};
    m_ptrRules[(i*2)+1] = {(uint8_t)(i+1), 19, 0, SCHEDULE_CLOSE};
  }

  SCHEDULE_DbgMsg("Initialized\n");
}

void class_schedule::Tick1min(void)
{
  m_MinuteFlag = true;
}
