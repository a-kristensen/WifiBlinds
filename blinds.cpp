#include "blinds.h"
#include "main.h"
#include "class_defs.h"


#define MOTOR_DBG
#ifdef MOTOR_DBG
  #define MOTOR_DbgMsg DbgMsg("Blinds: "); DbgMsg
#else
  #define MOTOR_DbgMsg
#endif


class_UserOfBlinds::class_UserOfBlinds(void)
{
  p_blinds = class_blinds::getInstance();
}



class_blinds::class_blinds(void)
{
  m_ptrMotdrv = new class_motdrv();
  m_ptrPos = new class_position_timebased(m_ptrMotdrv);

  //register to motor events
  m_ptrMotdrv->Attach(this);
}

void class_blinds::Tick1ms(void)
{ }

void class_blinds::TaskRun(void)
{
  //bottom limit
  if ((m_ptrPos->GetAbsPos() < 0) && (m_MotorSt == STATE_MOTOR_RUN_CLOSE))
  {
    Stop();
    m_CurtainSt = STATE_CURTAIN_BOTTOM;
    MOTOR_DbgMsg("in bottom limit\n");
  }
  
  //position is requested
  if (m_PosCtrl == true)
  {
    //check range, since position can come from both sides
    if((m_ptrPos->GetAbsPos() >= (m_ReqPos - 5)) &&
    ((m_ptrPos->GetAbsPos() <= (m_ReqPos + 5))))
    {
      Stop();
      m_CurtainSt = STATE_CURTAIN_USERPOS;
      MOTOR_DbgMsg("in requested position: %3.1f\n", m_ptrPos->GetAbsPos());
    }
  }
  
  //max limit
  if ((m_ptrPos->GetAbsPos() >= 100) && (m_MotorSt == STATE_MOTOR_RUN_OPEN))
  {
    Stop();
    m_CurtainSt = STATE_CURTAIN_MAX;
    MOTOR_DbgMsg("in max limit: %3.1f\n", m_ptrPos->GetAbsPos());
  }
}

void class_blinds::TaskInit(void)
{
  m_PosCtrl = false;
  m_ReqPos = 0;
  m_MotorSt = STATE_MOTOR_STOP;
  m_CurtainSt = STATE_CURTAIN_BOTTOM;

  MOTOR_DbgMsg("Initialized\n");
}

void class_blinds::m_Open(void)
{
  if (m_ptrPos->GetAbsPos() < 100)
  {
    m_MotorSt = STATE_MOTOR_RUN_OPEN;
    m_PosCtrl = false;
    m_CurtainSt = STATE_CURTAIN_RUN_OPEN;
    m_ptrMotdrv->Open();
  }
}

void class_blinds::m_Close(void)
{
  if (m_ptrPos->GetAbsPos() > 0)
  {
    m_MotorSt = STATE_MOTOR_RUN_OPEN;
    m_PosCtrl = false;
    m_CurtainSt = STATE_CURTAIN_RUN_CLOSE;
    m_ptrMotdrv->Close();
  }
}
void class_blinds::m_Stop(void)
{
  m_MotorSt = STATE_MOTOR_STOP;
  m_PosCtrl = false;
  m_ptrMotdrv->Stop();
}




void class_blinds::RunToPos(uint8_t pos_req)
{
  m_ReqPos = pos_req;

  MOTOR_DbgMsg("position requested (%u)\n", m_ReqPos);

  if(m_ReqPos < m_ptrPos->GetAbsPos())
    Close();
  if(m_ReqPos > m_ptrPos->GetAbsPos())
    Open();
  
  m_PosCtrl = true;
}

void class_blinds::ToogleOpenClose(uint8_t pos_req)
{
  if(m_ptrPos->GetAbsPos() == 0)
    RunToPos(pos_req);
  else
    Close();
}

uint8_t class_blinds::GetPos(void)
{
  return m_ptrPos->GetAbsPos();
}

CURTAIN_STATE class_blinds::GetCurtainState(void)
{
  return m_CurtainSt;
}


void class_blinds::Close(void)
{
  this->m_Close();
}
void class_blinds::Open(void)
{
  this->m_Open();
}

void class_blinds::Stop(void)
{
  this->m_Stop();
  m_CurtainSt = STATE_CURTAIN_USERPOS;
}

void class_blinds::onEvent(EVENT_TYPE e, unsigned int d)
{
  if(EVENT_MOTOR_DRV == e)
    m_MotorSt = (MOTOR_STATE)d;
}

//                           blinds

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//                           motor driver

class_motdrv::class_motdrv(void)
{
  pinMode(PIN_IA, OUTPUT);
  pinMode(PIN_IB, OUTPUT);
  m_MotorSt = STATE_MOTOR_STOP;
}

void class_motdrv::Close(void)
{
  digitalWrite(PIN_IA, HIGH);
  m_MotorSt = STATE_MOTOR_RUN_CLOSE;
  digitalWrite(PIN_IB, LOW);
  Notify(EVENT_MOTOR_DRV, (unsigned int)EVENT_MOTOR_CLOSING);
  MOTOR_DbgMsg("RUN_CLOSE\n");
}

void class_motdrv::Open(void)
{
  digitalWrite(PIN_IA, LOW);
  m_MotorSt = STATE_MOTOR_RUN_OPEN;
  digitalWrite(PIN_IB, HIGH);
  Notify(EVENT_MOTOR_DRV, (unsigned int)EVENT_MOTOR_OPENING);
  MOTOR_DbgMsg("RUN_OPEN\n");
}

void class_motdrv::Stop(void)
{
  digitalWrite(PIN_IA, LOW);
  m_MotorSt = STATE_MOTOR_STOP;
  digitalWrite(PIN_IB, LOW);
  Notify(EVENT_MOTOR_DRV, (unsigned int)EVENT_MOTOR_STOPPED);
  MOTOR_DbgMsg("STOP\n");
}

MOTOR_STATE class_motdrv::GetMotorState(void)
{
  return m_MotorSt;
}


//                           motor driver

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

//                           position


class_position_timebased::class_position_timebased(class_motdrv* motdrv)
{
  m_posTck = 0;
  m_maxTck = POS_MAX_TCK_OPEN;

  m_state = STATE_MOTOR_STOP;
  motdrv->Attach(this);
  m_calcFactor();
}

void class_position_timebased::m_calcFactor(void)
{
  m_factor = (double)100 / m_maxTck;
}

void class_position_timebased::Tick1ms(void)
{
  //maintain position
  switch(m_state)
  {
    case STATE_MOTOR_RUN_OPEN:   m_posTck++;  break;
    case STATE_MOTOR_RUN_CLOSE:  m_posTck--;  break;
    default: break;
  }
}

void class_position_timebased::onEvent(EVENT_TYPE t, unsigned int d)
{
  if(t == EVENT_MOTOR_DRV)
  {
    m_state = (MOTOR_STATE)d;
  }
}

double class_position_timebased::GetAbsPos(void)
{
  return m_posTck * m_factor;
}

void class_position_timebased::Calib_ResetPos(void)
{
  m_posTck = 0;
}
void class_position_timebased::Calib_SetMax(void)
{
  m_maxTck = m_posTck;
  m_calcFactor();
}

double class_position_timebased::GetRaw(void)
{
  return (double)m_posTck;
}
