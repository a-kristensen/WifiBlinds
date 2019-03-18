#ifndef _THINGER_H
#define _THINGER_H

//#include <ESP8266WiFi.h>
//#include <ThingerESP8266.h>
#include <stdint.h>
#include "class_defs.h"
#include "blinds.h"


class class_thinger : public Task, public class_UserOfBlinds
{
  private:

  //ThingerWifi thing("username", "deviceId", "deviceCredential");

  public:
    class_thinger();

    //inherited
    void TaskRun(void);
    void TaskInit(void);

};




#endif //_THINGER_H
