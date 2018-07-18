#include "observer.h"


void Subject::Notify(EVENT_TYPE t, unsigned int d)
{
  for (auto o = m_obs.begin(); o != m_obs.end(); ++o)
    (*o)->Update(t, d);
}

void Subject::Attach(Observer *obs)
{
  m_obs.push_back(obs);
}
