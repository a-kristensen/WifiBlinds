#include "main.h"
#include "param.h"
#include <EEPROM.h>

#define PARAM_DBG
#ifdef PARAM_DBG
  #define PARAM_DbgMsg(...) DbgMsg("PARAM: "); DbgMsg(__VA_ARGS__)
#else
  #define PARAM_DbgMsg
#endif

MEMORY_ITEM mem_map[] = {
  {MEMID_WIFI_SSID,    T_STR20,    345}
};




class_param::class_param(void) {}


void class_param::TaskInit(void)
{
  //this will read 512 bytes from the SPI flash
 // EEPROM.begin(512);

  PARAM_DbgMsg("Initialized\n");
}

void class_param::TaskRun(void)
{

}

void class_param::SetId(MEMORY_ID id, vector<uint8_t> buf)
{

}

void class_param::GetId(MEMORY_ID id, uint8_t* dptr, uint8_t len = 1)
{
  switch(id)
  {
    case MEMID_HOSTNAME:
      strncpy((char*)dptr, "Blinds_door", len);
    break;
    default:
      return;
  }
}

void Save(void)
{
 // EEPROM.commit();
}
