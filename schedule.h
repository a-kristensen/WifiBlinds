#ifndef _SCHEDULE_h
#define _SCHEDULE_h

#include <stdint.h>
#include "class_defs.h"
#include "datetime.h"
#include "blinds.h"


typedef struct
{
  uint8_t wd;
  uint8_t h;
  uint8_t m;
  uint8_t cmd;
} rule_item;

enum WeekDay
{
  MONDAY = 1,
  TUESDAY = 2,
  WEDNESDAY = 3,
  THURSDAY = 4,
  FRIDAY = 5,
  SATURDAY = 6,
  SUNDAY = 7
};

enum ScheduleCmd
{
  SCHEDULE_CLOSE = 1,
  SCHEDULE_OPEN = 2
};

const char* const StrWeekDay[] = 
{
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday",
  "Sunday"
};


typedef enum {
  FIRST_VAL = 0,
  SECOND_VAL
} someEnum1_t;

typedef enum {
  FIRST22_VAL = 0,
  SECOND22_VAL
} someEnum2_t;

class class_schedule : public Task, public Timer1min, public class_UserOfBlinds
{
  private:
    bool              m_MinuteFlag;
    bool              m_RulesActive;
    uint8_t           m_noOfRules;
    tm                m_currT;
    rule_item*        m_ptrRules;
    class_datetime**  m_datetime;
  
  public:
    class_schedule(class_datetime*&);

    //inherited
    void TaskRun(void);
    void TaskInit(void);
    void Tick1min(void);
};


#endif
