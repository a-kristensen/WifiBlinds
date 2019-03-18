#ifndef _WEB_h
#define _WEB_h

#include <ESP8266WebServer.h>
#include "arduino.h"
#include "main.h"
#include "blinds.h"
#include "datetime.h"
#include "param.h"


class class_web : public Task, public class_UserOfBlinds
{
  private:
    class_datetime**  mPtr_dt;
    class_param**     mPtr_param;
    ESP8266WebServer  m_WebServ;
    
    void handleRoot(void);
    void handleRoot2(void);
    void handleNotFound(void);

    void handleCmdOpen(void);
    void handleCmdClose(void);
    void handleGetPos(void);
    void handleCmdStop(void);
   
    void parseArgs(void);

    void update1(void);
    void update2(void);

  public:
    class_web(class_datetime*&, class_param*&);
    void TaskRun(void);
    void TaskInit(void);
};


#endif
