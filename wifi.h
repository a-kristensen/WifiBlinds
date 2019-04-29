#ifndef _WIFI_h
#define _WIFI_h

#include "class_defs.h"
#include "param.h"

typedef enum WIFI_STATE {
  STATE_WIFI_INITIATE,
  STATE_WIFI_DISCONNECTED,
  STATE_WIFI_CONNECTING,
  STATE_WIFI_CONNECTED,
  STATE_WIFI_STOP
} WIFI_STATE;


class class_wifi : public Task, public Subject<WIFI_STATE>
{
  private:
    class_param**  mPtr_param;
    const char* m_ssid = "Deathstar";
    const char* m_password = "hailtothekingbaby";
    const char* m_hostname = "Door_blinds";
    WIFI_STATE m_wifiState;

  public:
    class_wifi(class_param*&);
    void TaskRun(void);
    void TaskInit(void);
};

#endif



