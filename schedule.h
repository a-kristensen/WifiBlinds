#ifndef _SCHEDULE_h
#define _SCHEDULE_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif

#define NO_OF_RULES 14

typedef struct
{
  unsigned char wd;
  unsigned char h;
  unsigned char m;
  unsigned char cmd;
  unsigned char ena;
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

void InitSchedule(void);
void ScheduleTask(void);

extern unsigned char ScheduleMinFlag;

#endif

