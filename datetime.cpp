#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "main.h"
#include "datetime.h"
#include "schedule.h"
#include "debug.h"

#define DATETIME_DBG
#ifdef DATETIME_DBG
  #define DATETIME_DbgMsg DbgMsg
#else
  #define DATETIME_DbgMsg
#endif

unsigned long sendNTPpacket(IPAddress&);
void ParseNTPpacket(void);

unsigned char days_in_months[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
DateTime dt;

WiFiUDP udp;
byte ntpBuf[NTP_PACKET_SIZE];
IPAddress timeServer(77, 243, 43, 213); 


unsigned char retry = 0;
unsigned char midnight = 0;
unsigned long wait2sec = 0;
NTP_STATE state = NTP_SEND_REQ;
int parse;

void DTTask(void)
{
  
  
  switch(state)
  {
    case NTP_SEND_REQ:
      sendNTPpacket(timeServer);
      state = NTP_WAITFOR_REPLY;
      wait2sec = millis() + 2000;
    break;
    case NTP_WAITFOR_REPLY:
      parse = udp.parsePacket();
      //response received
      if(parse)
      {
        ParseNTPpacket();
        state = NTP_WAITFOR_MIDNIGHT;
      }
      //no response yet. has 2sec passed?
      else if(wait2sec < millis())
      {
        DATETIME_DbgMsg("NTP: udp req timeout: %d\r\n", retry);
        //retry again
        if(retry++ < 5)
          state = NTP_SEND_REQ;
        //tried 5 times, without reply. Don't try again
        else
        {
          DATETIME_DbgMsg("NTP: udp %u retries timeout\r\n", retry);
          state = NTP_WAITFOR_MIDNIGHT;
        }
      }
    break;
    case NTP_WAITFOR_MIDNIGHT:
      if(midnight)
      {
        retry = 0;
        state = NTP_SEND_REQ;
        midnight = 0;
      }
      break;
    default:
      state = NTP_SEND_REQ;
  }
}

DateTime GetTime(void)
{
	return dt;
}

void SetTime(DateTime d)
{
  dt = d;
}

void AddSec(void)
{
	dt.sec++;
	if (dt.sec >= 60)
	{
    //minute flag for schedule
    ScheduleMinFlag = 1;
  
		dt.sec = 0;
		dt.min++;

		if (dt.min >= 60)
		{
			dt.min = 0;
			dt.hour++;

			if (dt.hour >= 24)
			{
//        midnight = 1;
				dt.hour = 0;

        dt.wday++;
        if(dt.wday > 7)
        {
          dt.wday = 1; //monday
        }

        //dt.day++;
				/*if (dt.day > months[dt.month])
				{
					dt.day = 0;
					dt.month++;

					if (dt.month > 12)
					{
						dt.month = 0;
						dt.year++;
					}
				}*/
			}
		}
	}
}

unsigned long sendNTPpacket(IPAddress& addr)
{
  if(WiFi.status() == WL_CONNECTED)
  {
    udp.begin(2390);
    memset(ntpBuf, 0, NTP_PACKET_SIZE);
  
    ntpBuf[0] = 0b11100011;   // LI, Version, Mode
    //ntpBuf[1] = 0;     // Stratum, or type of clock
    ntpBuf[2] = 6;     // Polling Interval
    ntpBuf[3] = 0xEC;  // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    ntpBuf[12]  = 49;
    ntpBuf[13]  = 0x4E;
    ntpBuf[14]  = 49;
    ntpBuf[15]  = 52;
  
    udp.beginPacket(addr, 123);
    udp.write(ntpBuf, NTP_PACKET_SIZE);
    udp.endPacket();

    DATETIME_DbgMsg("NTP: udp req sended\r\n");
  }
  else
    DATETIME_DbgMsg("NTP: No NET\r\n");
}

/*
void ParseNTPpacket(void)
{
  DateTime dt;
  udp.read(ntpBuf, NTP_PACKET_SIZE);

  //secs since 1.jan 1900
  unsigned long t_sec = (ntpBuf[40]<<24) + (ntpBuf[41]<<16) + (ntpBuf[42]<<8) + ntpBuf[43];
  //secs since 1970
  t_sec = t_sec - YEARS_70 - JAN2007;

  dt.sec = (t_sec % 60);
  dt.min = ((t_sec  % SECS_IN_HOUR) / 60);
  dt.hour = ((t_sec  % SECS_IN_DAY) / SECS_IN_HOUR) + 2;
  dt.wday = ((t_sec % (7*SECS_IN_DAY)) / SECS_IN_DAY) + 1; //weekday
  
  SetTime(dt);
  validTime = 1;
  DATETIME_DbgMsg("NTP: %s - %2u:%2u:%2u\n", StrWeekDay[dt.wday-1], dt.hour, dt.min, dt.sec);
}*/


bool SummerTime(DateTime dt0)
{
  //months between march and october are summertime
  //last saturday in march starts summertime (not the 31.)
  //last saturday in october stops summertime (not the 31.)
  if(dt0.month > 2 && dt0.month < 9)
    return true;

  return false;
}


void ParseNTPpacket(void)
{
  DateTime dt;
  udp.read(ntpBuf, NTP_PACKET_SIZE);

  //secs since 1.jan 1900
  unsigned long t_sec = (ntpBuf[40]<<24) + (ntpBuf[41]<<16) + (ntpBuf[42]<<8) + ntpBuf[43];
  //secs since 1970
  t_sec = t_sec - YEARS_70 - JAN2017;

  //how many years passed since 2017
  dt.year = 2017 + (t_sec / (SECS_IN_DAY * DAYS_IN_YEAR));

  //januar 2017, 1. jan = sunday ~ offset
  //weekday
  dt.wday = ((t_sec - SECS_IN_DAY) % (7*SECS_IN_DAY)) / SECS_IN_DAY;

  //subtract seconds for each year
  t_sec = t_sec % (SECS_IN_DAY * DAYS_IN_YEAR);
  //january
  dt.month = 0;


  //calculate months
  for(int i=0; i<12; i++)
  {
    if(t_sec / (days_in_months[i] * SECS_IN_DAY))
    {
      t_sec -= days_in_months[i] * SECS_IN_DAY;
      dt.month++;
    }
    else
      break;
  }

  //calculate days
  dt.day = t_sec / SECS_IN_DAY;
  t_sec %= SECS_IN_DAY;

  dt.sec = (t_sec % 60);
  dt.min = ((t_sec % SECS_IN_HOUR) / 60);
  dt.hour = (t_sec / SECS_IN_HOUR);

  if(SummerTime)
    dt.hour += 2;
  else
    dt.hour += 1;


  SetTime(dt);
  validTime = 1;

  DATETIME_DbgMsg("NTP: %s d. %2u. %s - %2u\r\n",StrWeekDay[dt.wday], dt.day+1, StrMonth[dt.month], dt.year);
  DATETIME_DbgMsg("NTP: %02u:%02u:%02u\r\n", dt.hour, dt.min, dt.sec); 
}


