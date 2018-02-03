#include "main.h"
#include "debug.h"
#include <EEPROM.h>

//#define MEM_DBG
#ifdef MEM_DBG
  #define MEM_DbgMsg DbgMsg
#else
  #define MEM_DbgMsg
#endif

void readBytes(unsigned int addr, unsigned int size, unsigned char* data)
{
  for(; size>0; addr++, size--)
    *data = EEPROM.read(addr);
}

void writeBytes(unsigned int addr, unsigned int size, unsigned char* data)
{
  for(; size>0; addr++, size--)
    EEPROM.write(addr, *data);
}

void CommitEEPROM(void)
{
  EEPROM.commit();
}



void InitMem(void)
{
  //creates tmp array and reads memory
  EEPROM.begin(0x64);

}




/*
  RulesActive = EEPROM.read(ADDR_TIMERULE_ACTIVE);

  if(RulesActive)
  {
    noOfRules = EEPROM.read(ADDR_TIMERULE_NOITEMS);
    ptrRules = new rule_item[noOfRules];

    for(int i=0; i<noOfRules*sizeof(rule_item); i++)
      *(i+(unsigned char*)ptrRules) = EEPROM.read(ADDR_TIMERULEITEMS+i);
  }*/



