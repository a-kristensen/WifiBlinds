#include "main.h"
#include "param.h"
#include <EEPROM.h>

#define PARAM_DBG
#ifdef PARAM_DBG
  #define PARAM_DbgMsg(...) DbgMsg("PARAM: "); DbgMsg(__VA_ARGS__)
#else
  #define PARAM_DbgMsg
#endif


MemoryItem_t memMap[] =
{
// item                size    addr
  {MEMID_WIFI_SSID,    20,       0},
  {MEMID_WIFI_PASS,    20,      20},
  {MEMID_SCHED_ENA,     1,      40},
  {MEMID_SCHED_RULES,  30,      41},
  {MEMID_MULTI_GROUP,   1,      42},
  {MEMID_IR_ENA,        1,      43},
  {MEMID_IR_CODE_THIS,  4,      44},
  {MEMID_IR_ALL,        4,      48},
  {MEMID_IR_GROUP_NUM,  1,      52},
  {MEMID_IR_GROUP_CODE, 1,      53},
  {MEMID_HOSTNAME,      20,     54},
  
  {MEMID_CRC_ADDR,      4,     251}
};




class_param::class_param(void) {}


void class_param::TaskInit(void)
{
  //this will read 512 bytes from the SPI flash
  EEPROM.begin(512);

  PARAM_DbgMsg("Initialized\n");
}

void class_param::TaskRun(void)
{

}

uint32_t class_param::calcCrc32OfMem(void)
{
  return 0;
}

uint32_t class_param::calcCrc32(uint8_t* buf, uint32_t byteSize)
{
  int i, j;
  unsigned int by, crc, mask;

  i = 0;
  crc = 0xFFFFFFFF;

  while(byteSize--)
  {
    // Get next byte
    by = buf[i];            
    crc = crc ^ by;

    // Do eight times
    for (j = 7; j >= 0; j--)
    {
      mask = -(crc & 1);
      crc = (crc >> 1) ^ (0xEDB88320 & mask);
    }
    i = i + 1;
  }
  return ~crc;
}

void class_param::saveAll()
{
  EEPROM.commit();
}

bool class_param::setId(MemoryId_t id, uint8_t* buf)
{
  MemoryItem_t m;
  bool idFound = false;
  memset(&m, 0, sizeof(MemoryItem_t));

  //find it in the table
  for(uint8_t i=0; i<(sizeof(memMap)/sizeof(MemoryItem_t)); i++)
  {
    if(memMap[i].addr == id)
    {
      m = memMap[i];
      idFound = true;
      break;
    }
  }

  if(idFound)
  {
    EEPROM.write(m.addr, m.size);
  }

  return idFound;
}

bool class_param::getId(MemoryId_t id, uint8_t* buf)
{


  switch(id)
  {
    case MEMID_HOSTNAME:
//      strncpy((char*)dptr, "Blinds_door", len);
    break;
    default:
      return false;
  }

  return false;
}
