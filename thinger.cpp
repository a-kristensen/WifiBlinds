#include "thinger.h"
#include "main.h"




#define THINGER_DBG
#ifdef THINGER_DBG
  #define THINGER_DbgMsg(...) DbgMsg("Thinger: "); DbgMsg(__VA_ARGS__)
#else
  #define MOTOR_DbgMsg
#endif


class_thinger::class_thinger()
{
  
  //thing.add_wifi("some_ssid", "some_password");

}

void class_thinger::TaskRun(void)
{
  //thing.handle();
}

void class_thinger::TaskInit(void)
{}


