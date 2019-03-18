#ifndef _DATETIME_h
#define _DATETIME_h


#include "class_defs.h"
#include <WiFiUdp.h>
#include <time.h>


//NTP time stamp is in the first 48 bytes of the message
#define NTP_PACKET_SIZE 48

//unix timestamp for 1.jan. 2007 (monday)
#define JAN2007 1167609600UL
//unix timestamp for 1.jan. 2018 (monday)
#define JAN2018 1514764800UL

#define YEARS_70 2208988800UL
#define SECS_IN_DAY 86400L
#define SECS_IN_HOUR 3600

/* struct tm
tm_sec    int   seconds after the minute  0-61*
tm_min    int   minutes after the hour    0-59
tm_hour   int   hours since midnight      0-23
tm_mday   int   day of the month          1-31
tm_mon    int   months since January      0-11
tm_year   int   years since 1900
tm_wday   int   days since Sunday         0-6
tm_yday   int   days since January 1      0-365
tm_isdst  int   Daylight Saving Time flag 
*/


typedef enum
{
  NTP_SEND_REQ = 0,
  NTP_WAITFOR_REPLY = 1,
  NTP_WAITFOR_MIDNIGHT = 2,
} NTP_STATE;


class class_datetime : public Task, public Timer1sec
{
  private:
    void      sendNTPpacket(char* addr);
    void      ParseNTPpacket(void);
    struct    tm m_dt;
    char*     m_timeServer;
    uint8_t   m_retry;
    volatile  bool m_midnight;
    unsigned  long m_wait2sec;
    int       m_parse;

    WiFiUDP   m_udp;
    uint8_t   ntpBuf[NTP_PACKET_SIZE];
    
    bool      m_ValidTime;

  public:
    class_datetime(void);
    tm        GetTime(void);
    void      SetTime(tm);
    bool      isTimeValid(void);

    void      TaskRun (void);
    void      TaskInit (void);
    void      Tick1sec(void);
};


#endif

