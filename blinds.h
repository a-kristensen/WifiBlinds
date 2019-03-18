#ifndef _BLINDS_H
#define _BLINDS_H

#include <stdint.h>
#include "class_defs.h"


#define POS_MAX_TCK_OPEN	10000
#define PIN_IA	MYPIN_D2
#define PIN_IB	MYPIN_D1

typedef enum CURTAIN_STATE
{
  STATE_CURTAIN_RUN_CLOSE,
  STATE_CURTAIN_RUN_OPEN,
  STATE_CURTAIN_USERPOS,
  STATE_CURTAIN_MAX,
  STATE_CURTAIN_BOTTOM
} CURTAIN_STATE;


typedef enum MOTOR_STATE
{
  STATE_MOTOR_STOP,
  STATE_MOTOR_RUN_OPEN,
  STATE_MOTOR_RUN_CLOSE
} MOTOR_STATE;

typedef enum MOTOR_EVENTS
{
  EVENT_MOTOR_STOPPED,
  EVENT_MOTOR_OPENING,
  EVENT_MOTOR_CLOSING
} MOTOR_EVENTS;


class class_motdrv;
class intf_position;
class class_position_timebased;
class class_blinds;

// this class is to inherit from, for every client that uses the blinds
class class_UserOfBlinds
{
  public:
    class_UserOfBlinds(void);
  protected:
    class_blinds* p_blinds;
};



//this is an abstraction layer
//it abstracts away from motor/postion to curtains
class class_blinds : public Task, public Timer1ms, public Subject, public Observer, public Singleton<class_blinds>
{
  private:
    void            m_Close(void);
    void            m_Open(void);
    void            m_Stop(void);
  
    intf_position*  m_ptrPos;
    class_motdrv*   m_ptrMotdrv;

    volatile MOTOR_STATE    m_MotorSt;
    volatile uint8_t        m_ReqPos;         // requested position (0 - 100)
    volatile bool           m_PosCtrl;        // state bit, if position has been requested

    CURTAIN_STATE           m_CurtainSt;

  public:
    class_blinds(void);
    void            Close(void);
    void            Open(void);
    void            Stop(void);
    void            RunToPos(uint8_t);   //input in %
    void            ToogleOpenClose(uint8_t);
    uint8_t         GetPos(void);     //output in %
    CURTAIN_STATE   GetCurtainState(void);

    //inherited
    void TaskRun(void);
    void TaskInit(void);
    void Tick1ms(void);
    void onEvent(EVENT_TYPE, unsigned int);
};



class class_motdrv : public Subject
{
  private:
    MOTOR_STATE     m_MotorSt;
    uint8_t         m_MotorSpeed;   //this will be set one time, and used on subsequent calls

  public:
    class_motdrv(void);
    void            Close(void);
    void            Open(void);
    void            Stop(void); 
    MOTOR_STATE     GetMotorState(void);
};


//position interface base class
class intf_position
{
  public:
    //absolute, linear position: 0 - 100
    virtual double      GetAbsPos(void) = 0;     
    virtual double      GetRaw(void) = 0;
    virtual void        Calib_ResetPos(void) = 0;
    virtual void        Calib_SetMax(void) = 0;
};


class class_position_timebased : public intf_position, public Timer1ms, public Observer
{
  private:
    volatile signed long  m_posTck;
    signed long           m_maxTck;
    double                m_factor;   //factor to convert from ticks to pos0-100
    MOTOR_STATE           m_state;

    void m_calcFactor(void);

  public:
    class_position_timebased(class_motdrv*);

    //inherited
    void        Tick1ms(void);
    void        onEvent(EVENT_TYPE, unsigned int);

    //inherited interface
    double      GetAbsPos(void);
    double      GetRaw(void);
    void        Calib_ResetPos(void);
    void        Calib_SetMax(void);
};




#endif //_BLINDS_H
