#include <ESP8266WiFi.h>
#include "main.h"
#include "datetime.h"
#include "schedule.h"

#define DATETIME_DBG
#ifdef DATETIME_DBG
  #define DATETIME_DbgMsg DbgMsg("Datetime: "); DbgMsg
#else
  #define DATETIME_DbgMsg
#endif


//IPAddress timeServer(77, 243, 43, 213); 
//char months[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
//char l_months[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};



void class_datetime::sendNTPpacket(char* addr)
{
  if(WiFi.status() == WL_CONNECTED)
  {
    m_udp.begin(8888);
    
    memset(ntpBuf, 0, NTP_PACKET_SIZE);
  
    ntpBuf[0] = 0b11100011;   // LI, Version, Mode
    //ntpBuf[1] = 0;          // Stratum, or type of clock
    ntpBuf[2] = 6;            // Polling Interval
    ntpBuf[3] = 0xEC;         // Peer Clock Precision
    //8 bytes of zero for Root Delay & Root Dispersion
    ntpBuf[12]  = 49;
    ntpBuf[13]  = 0x4E;
    ntpBuf[14]  = 49;
    ntpBuf[15]  = 52;
  
    m_udp.beginPacket(addr, 123);
    m_udp.write(ntpBuf, NTP_PACKET_SIZE);
    m_udp.endPacket();

    DATETIME_DbgMsg("NTP: udp request sent\n");
  }
  //else
  //  DATETIME_DbgMsg("NTP: No NET\n");
}
void class_datetime::ParseNTPpacket(void)
{
  tm dt;
  m_udp.read(ntpBuf, NTP_PACKET_SIZE);

  //secs since 1.jan 1900
  unsigned long t_sec = (ntpBuf[40]<<24) + (ntpBuf[41]<<16) + (ntpBuf[42]<<8) + ntpBuf[43];
  //secs since 1970
  t_sec = t_sec - YEARS_70 - JAN2018;

  dt.tm_sec = (t_sec % 60);
  dt.tm_min = ((t_sec  % SECS_IN_HOUR) / 60);
  dt.tm_hour = ((t_sec  % SECS_IN_DAY) / SECS_IN_HOUR) + 2;
  dt.tm_wday = ((t_sec % (7*SECS_IN_DAY)) / SECS_IN_DAY) + 1; //weekday
  
  SetTime(dt);
  m_ValidTime = true;
  DATETIME_DbgMsg("NTP: %s - %02u:%02u:%02u\n", StrWeekDay[dt.tm_wday-1], dt.tm_hour, dt.tm_min, dt.tm_sec);
}

class_datetime::class_datetime(void)
{
  m_retry = 0;
  m_midnight = false;
  m_wait2sec = 0;

  memset(&m_dt, 0, sizeof(tm));
}

tm class_datetime::GetTime(void)
{ 
  return m_dt;
}

void class_datetime::SetTime(tm d)
{
  m_dt = d;
}

bool class_datetime::isTimeValid(void)
{
  return m_ValidTime;
}

void class_datetime::TaskRun (void)
{
  static NTP_STATE state = NTP_SEND_REQ;
  
  switch(state)
  {
    case NTP_SEND_REQ:
      sendNTPpacket((char*)m_timeServer);
      state = NTP_WAITFOR_REPLY;
      m_wait2sec = millis() + 4000;
    break;
    case NTP_WAITFOR_REPLY:
      m_parse = m_udp.parsePacket();
      //response received
      if(m_parse)
      {
        ParseNTPpacket();
        state = NTP_WAITFOR_MIDNIGHT;
      }
      //no response yet. has 2sec passed?
      else if(m_wait2sec < millis())
      {
        //DATETIME_DbgMsg("NTP:udp req timeout: %d\n", retry);
        //retry again
        if(m_retry++ < 5)
          state = NTP_SEND_REQ;
        //tried 5 times, without reply. Don't try again
        else
        {
          DATETIME_DbgMsg("NTP:udp %u retries timeout\n", m_retry);
          state = NTP_WAITFOR_MIDNIGHT;
        }
      }
    break;
    case NTP_WAITFOR_MIDNIGHT:
      if(m_midnight)
      {
        m_retry = 0;
        state = NTP_SEND_REQ;
        m_midnight = false;
      }
      break;
    default:
      state = NTP_SEND_REQ;
  }
}

void class_datetime::TaskInit (void)
{
  m_timeServer = (char*)"dk.pool.ntp.org";
  m_ValidTime = false;

  DATETIME_DbgMsg("Initialized\n");
}

void class_datetime::Tick1sec(void)
{
  m_dt.tm_sec++;
  if (m_dt.tm_sec >= 60)
  {  
    m_dt.tm_sec = 0;
    m_dt.tm_min++;

    if (m_dt.tm_min >= 60)
    {
      m_dt.tm_min = 0;
      m_dt.tm_hour++;

      if (m_dt.tm_hour >= 24)
      {
        m_midnight = true;
        m_dt.tm_hour = 0;

        m_dt.tm_wday++;
        if(m_dt.tm_wday > 7)
        {
          m_dt.tm_wday = 1; //monday
        }

        /*m_dt.tm_mday++;
        if (m_dt.tm_mday > months[dt.month])
        {
          m_dt.tm_mday = 0;
          m_dt.tm_mon++;

          if (m_dt.tm_mon > 12)
          {
            m_dt.tm_mon = 0;
            m_dt.tm_year++;
          }
        }*/
      }
    }
  }
}

