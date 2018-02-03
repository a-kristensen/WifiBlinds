#ifndef _DATETIME_h
#define _DATETIME_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif

//NTP time stamp is in the first 48 bytes of the message
#define NTP_PACKET_SIZE 48

//unix timestamp for 1.jan. 2007 (monday)
#define JAN2007 1167609600UL

//unix timestamp for 1.jan 2017
#define JAN2017 1483228800UL

#define YEARS_70 2208988800UL
#define SECS_IN_DAY 86400L
#define SECS_IN_HOUR 3600

#define DAYS_IN_YEAR 365
#define DAYS_IN_LEAPYEAR 366

typedef struct {
	unsigned char sec;
	unsigned char min;
	unsigned char hour;
  unsigned char wday;
  
  unsigned char day;
	unsigned char month;
	unsigned int  year;
} DateTime;

typedef enum
{
  NTP_SEND_REQ = 0,
  NTP_WAITFOR_REPLY = 1,
  NTP_WAITFOR_MIDNIGHT = 2,
} NTP_STATE;


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

const char* const StrMonth[] = 
{
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December"
};

void DTTask(void);
void AddSec(void);
DateTime GetTime(void);
void SetTime(DateTime);

#endif

