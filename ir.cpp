#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include "ir.h"
#include "main.h"


// only use this version: _IRREMOTEESP8266_VERSION_  2.4.2



#define IR_DBG
#ifdef IR_DBG
  #define IR_DbgMsg(...) DbgMsg("IR: "); DbgMsg(__VA_ARGS__)
#else
  #define IR_DbgMsg
#endif


class_ir::class_ir()
{ }

void class_ir::Tick1ms(void)
{
  static uint8_t Cnt1ms = 0;
  
  //samsung remote repeat time: 107-109 ms
  //ir remote button repeat
  if (Cnt1ms++ >= 120)
  {
    m_FlagBtnRepeat = true;
    Cnt1ms = 0;
  }
}

char* class_ir::getBtnString(unsigned long id)
{
  const char* r;

  switch(id)
  {
    case SAMSUNG_BTN_RED:
      r = btn_red;
    break;
    case SAMSUNG_BTN_GREEN:
      r = btn_green;
    break;
    case SAMSUNG_BTN_YELLOW:
      r = btn_yellow;
    break;
    case SAMSUNG_BTN_BLUE:
      r = btn_blue;
    break;
    default:
      r = btn_unknown;
    break;
    //char const* const 
  }

  return (char*)r;

  //return (char*)btn_blue;
}

void class_ir::TaskRun(void)
{
  static unsigned long btnid = 0;
  static char* btnstr = (char*)getBtnString(0);

  //has a button been received
  if(m_IRRecv->decode(&m_IRRes))
  {
    //IR_DbgMsg("code: %lu\n", m_IRRes.value);
    m_IRRecv->resume();

    //only accept samsung color buttons
    if(m_IRRes.value == SAMSUNG_BTN_RED || m_IRRes.value == SAMSUNG_BTN_BLUE || m_IRRes.value == SAMSUNG_BTN_GREEN || m_IRRes.value == SAMSUNG_BTN_YELLOW)
    {
      m_btnPress = true;
    }
  }
  
  //button repeat time
  if (m_FlagBtnRepeat)
  {
    //has button been pressed
    if(m_btnPress)
    {
      btnid = m_IRRes.value;
      btnstr = (char*)getBtnString(btnid);
      
      m_btnDown = true;
      m_btnPress = false;

      if(m_firstpress)
      {
        handleButton({btnid, BTN_PRESS});
        IR_DbgMsg("PRESS: %s (%lu)\n", btnstr, btnid);
        m_firstpress = false;
      }
      else
      {
        handleButton({btnid, BTN_REPEAT});
        IR_DbgMsg("REPEAT: %s (%lu)\n", btnstr, btnid);
      }
    }
    //has button been pressed previously
    else if(m_btnDown == true)
    {
      handleButton({btnid, BTN_RELEASE});
      IR_DbgMsg("RELEASE: %s (%lu)\n", btnstr, btnid);
      btnid = 0;
      m_firstpress = true;
      m_btnDown = false;
    }

    m_FlagBtnRepeat = false;
  }
}


void class_ir::handleButton(BUTTON_EVENT btn)
{
  static BUTTON_STATE state = WAITING;
  static bool toggle = false;

  if(btn.Id == SAMSUNG_BTN_BLUE)
  {
    if(btn.State == BTN_PRESS)
    {
      if(p_blinds->GetPos() != 0)
      {
        p_blinds->Close();
        IR_DbgMsg("MASTER:close\n");
      }
      else
      {
        p_blinds->Open();
        IR_DbgMsg("MASTER:open\n");
      }
    }
  }

  else if(btn.Id == SAMSUNG_BTN_GREEN)
  {
    switch(state)
    {
      case WAITING:
        //long press
        if( btn.State == BTN_REPEAT)
        {
          //blinds are in open position
          if(p_blinds->GetPos() != 0)
          {
            if(toggle)
            {
              p_blinds->Close();
              IR_DbgMsg("long_press_toggle (closing)\n");
            }
            else
            {
              p_blinds->Open();
              IR_DbgMsg("long_press_toggle (opening)\n");
            }
            toggle = !toggle;
          }
          //blinds are in closed position
          else
          {
            p_blinds->Open();
            IR_DbgMsg("long_press_start (opening)\n"); 
          }
  
          state = WAITING_FOR_RELEASE;
        }
        //short press (no previous REPEAT)
        else if( btn.State == BTN_RELEASE)
        {
          IR_DbgMsg("short_press_release (toggle position)\n");   
          p_blinds->ToogleOpenClose(m_IrUserPos);
        }
      break;
      case WAITING_FOR_RELEASE:
        if( btn.State == BTN_RELEASE)
        {
          p_blinds->Stop();
          IR_DbgMsg("long_press_release(stop)\n");
          state = WAITING;
        }
      break;
      default:
        state = WAITING;
    }
  }
}


void class_ir::TaskInit (void)
{
  m_IRRecv = new IRrecv(MYPIN_D5);

  //m_motDrv = ins_motor;
  
  m_btnPress = false;
  m_firstpress = true;
  m_btnDown = false;
  
  m_IRRecv->enableIRIn(); // Start the receiver

  IR_DbgMsg("Initialized\n");
}
