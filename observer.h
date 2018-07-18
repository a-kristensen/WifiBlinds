#ifndef _OBSERVER_h
#define _OBSERVER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include <vector>
using namespace std;

typedef enum EVENT_TYPE
{
  EVENT_MOTOR = 0,
  EVENT_IR = 1,
  EVENT_MQTT = 2,
  EVENT_WEB = 3,
  EVENT_SCHED = 4,
  EVENT_WIFI = 5
} EVENT_TYPE;

class Observer;

class Subject
{
  private:
	  vector <Observer*> m_obs;

  public:
    void Attach(Observer *o);
	  void Detach(Observer *o);

  protected:
	  void Notify(EVENT_TYPE t, unsigned int d);
};
	
class Observer
{
  public:
	  virtual void Update(EVENT_TYPE t, unsigned int d) = 0;
};



#endif

