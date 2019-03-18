#ifndef _IR_h
#define _IR_h

//#include "arduino.h"
#include <string>
#include "class_defs.h"
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include "blinds.h"


typedef enum BUTTON_ACTION {
  BTN_NOACTION,
  BTN_PRESS,
  BTN_REPEAT,
  BTN_RELEASE
} BUTTON_ACTION;

typedef enum BUTTON_STATE {
  WAITING,
  WAITING_FOR_RELEASE,
} BUTTON_STATE;

typedef struct {
	unsigned long Id;
	BUTTON_ACTION State;
} BUTTON_EVENT;


/*
#define BTN_RED     0xDAEA83EC
#define BTN_GREEN   0x2BAFCEEC
#define BTN_YELLOW  0xB5210DA6
#define BTN_BLUE    0x71A1FE88
*/

#define SAMSUNG_BTN_RED       0xE0E036C9
#define SAMSUNG_BTN_GREEN     0xE0E028D7
#define SAMSUNG_BTN_YELLOW    0xE0E0A857
#define SAMSUNG_BTN_BLUE      0xE0E06897




class class_ir : public Task, public Timer1ms, public class_UserOfBlinds
{
  private:
    void handleButton(BUTTON_EVENT);
    char* getBtnString(unsigned long);
    IRrecv* m_IRRecv;
    decode_results m_IRRes;
    volatile bool m_FlagBtnRepeat;
    bool m_btnPress;
    bool m_firstpress;
    bool m_btnDown;
    
    const unsigned char m_IrUserPos = 50;
    char const* const btn_red = "Samsung_red";
    char const* const btn_green = "Samsung_green";
    char const* const btn_yellow = "Samsung_yellow";
    char const* const btn_blue = "Samsung_blue";
    char const* const btn_unknown = "unknown";
    
  public:
    class_ir();
    
    void Tick1ms(void);
    void TaskRun (void);
    void TaskInit (void);
};


#endif
