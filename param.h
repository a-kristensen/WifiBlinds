#ifndef _PARAM_h
#define _PARAM_h

#include <vector>
#include <stdint.h>
#include "class_defs.h"
#include "param.h"


typedef enum MEMORY_ID {
  MEMID_WIFI_SSID,
  MEMID_WIFI_PASS,
  MEMID_HOSTNAME
} MEMORY_ID;

typedef enum MEMORY_TYPE {
  T_BYTE = 0,
  T_INT = 1,
  T_FLOAT = 2,
  T_STR20 = 3
} MEMORY_TYPE;

typedef struct {
  MEMORY_ID     Id;
  MEMORY_TYPE   Type;
  unsigned int  Addr;
} MEMORY_ITEM;



class class_param : public Task
{
  private:


  public:
    class_param(void);
    void TaskInit(void);
    void TaskRun(void);

    void GetId(MEMORY_ID, uint8_t* dptr, uint8_t len);
    void SetId(MEMORY_ID, vector<uint8_t>);
    void Save(void);
};




#endif

