#ifndef _PARAM_h
#define _PARAM_h

#include <vector>
#include <stdint.h>
#include "class_defs.h"
#include "param.h"


typedef enum {
  MEMID_SCHED_ENA,          // enable
  MEMID_SCHED_RULES,        // all rules
////////////////////////
  MEMID_MULTI_GROUP,        // member of group
////////////////////////
  MEMID_IR_ENA,             // enable
  MEMID_IR_CODE_THIS,       // ir code to control the internal motor
  MEMID_IR_ALL,             // ir code to control all blinds on network
  MEMID_IR_GROUP_NUM,       // group num to be controlled by group code
  MEMID_IR_GROUP_CODE,      // ir code for group number
////////////////////////
  MEMID_WIFI_SSID,          // wifi ssid
  MEMID_WIFI_PASS,          // password
////////////////////////
  MEMID_HOSTNAME,           // name of blinds
////////////////////////
  MEMID_CRC_ADDR            // crc for all memory id's 
} MemoryId_t;

typedef struct {
  MemoryId_t  Id;
  uint8_t     size;
  uint8_t     addr;
} MemoryItem_t;



class class_param : public Task
{
  private:

    bool findAddrId(MemoryId_t);
    uint32_t calcCrc32(uint8_t*, uint32_t);
    uint32_t calcCrc32OfMem(void);

  public:
    class_param(void);
    void TaskInit(void);
    void TaskRun(void);

    bool getId(MemoryId_t, uint8_t* buf);
    bool setId(MemoryId_t, uint8_t* buf);
    void saveAll(void);
};




#endif

