#include <ESP8266WiFi.h>
#include "wifi.h"
#include "main.h"


#define WIFI_DBG
#ifdef WIFI_DBG
  #define WIFI_DbgMsg DbgMsg("Wifi: "); DbgMsg
#else
  #define WIFI_DbgMsg
#endif



class_wifi::class_wifi(class_param*& ref_param)
{
  m_wifiState = STATE_WIFI_INITIATE;
  mPtr_param = &ref_param;
}


void class_wifi::TaskRun(void)
{

  switch(m_wifiState)
  {
    case STATE_WIFI_INITIATE:
      //WiFi instantiated in ESP8266WiFi.cpp
      WiFi.begin(m_ssid, m_password);
      m_wifiState = STATE_WIFI_CONNECTING;
      WIFI_DbgMsg("connecting...\n");
    break;
    case STATE_WIFI_CONNECTING:
      //wait for connection
      if(WiFi.status() == WL_CONNECTED)
      {
        m_wifiState = STATE_WIFI_CONNECTED;
        WIFI_DbgMsg("connected (ip: %s, gw: %s, ssid: %s)\n", WiFi.localIP().toString().c_str(), WiFi.gatewayIP().toString().c_str(), m_ssid);
      }
    break;    
    case STATE_WIFI_CONNECTED:
    
      if(WiFi.status() != WL_CONNECTED)
      {
        m_wifiState = STATE_WIFI_DISCONNECTED;
        WIFI_DbgMsg("disconnected\n");
      }
    break;
    case STATE_WIFI_DISCONNECTED:
      //when the wifi disconnects, it automatically try to reconnect
    break;
    case STATE_WIFI_STOP:
      // do nothing here.
    break;
  }
}

void class_wifi::TaskInit(void)
{
  char str_hostname[20] = {0};
  (**mPtr_param).GetId(MEMID_HOSTNAME, (unsigned char*)str_hostname, 20);

  WiFi.hostname(str_hostname);
  WiFi.mode(WIFI_STA);
  WIFI_DbgMsg("Initialized\n");
}

