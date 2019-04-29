#include "class_defs.h"
#include "arduino.h"

//static vectors need initialization
vector<Task*> TaskScheduler::m_taskList;
vector<Timer1ms*> TaskScheduler::m_Timer1msList;
vector<Timer1sec*> TaskScheduler::m_Timer1secList;
vector<Timer1min*> TaskScheduler::m_Timer1minList;


void TaskScheduler::regTask(Task* t)
{
  m_taskList.push_back(t);
}
void TaskScheduler::regTimer1ms(Timer1ms* t)
{
  m_Timer1msList.push_back(t);
}

void TaskScheduler::regTimer1sec(Timer1sec* t)
{
  m_Timer1secList.push_back(t);
}

void TaskScheduler::regTimer1min(Timer1min* t)
{
  m_Timer1minList.push_back(t);
}





void TaskScheduler::runSchedOnce(void)
{
  for (auto t = m_taskList.begin(); t != m_taskList.end(); ++t)
    (*t)->TaskRun();
}

void TaskScheduler::initTasks(void)
{
  for (auto t = m_taskList.begin(); t != m_taskList.end(); ++t)
    (*t)->TaskInit();
}

Task::Task(void)
{
  TaskScheduler::regTask(this);
}

Timer1ms::Timer1ms(void)
{
  TaskScheduler::regTimer1ms(this);
}

Timer1sec::Timer1sec(void)
{
  TaskScheduler::regTimer1sec(this);
}

Timer1min::Timer1min(void)
{
  TaskScheduler::regTimer1min(this);
}

void TaskScheduler::runAllTimer1ms(void)
{
  for (auto t = m_Timer1msList.begin(); t != m_Timer1msList.end(); ++t)
    (*t)->Tick1ms();
}
void TaskScheduler::runAllTimer1sec(void)
{
  for (auto t = m_Timer1secList.begin(); t != m_Timer1secList.end(); ++t)
    (*t)->Tick1sec();
}

void TaskScheduler::runAllTimer1min(void)
{
  for (auto t = m_Timer1minList.begin(); t != m_Timer1minList.end(); ++t)
    (*t)->Tick1min();
}






TimedTask::TimedTask(Task* t)
{
  ptr_task = t;
}

void TimedTask::TaskRun(void)
{
  m_prevUs = micros();
  ptr_task->TaskRun();
  m_prevUs = micros() - m_prevUs;

  if(m_prevUs > m_maxUs)
    m_maxUs = m_prevUs;
  if(m_prevUs < m_minUs)
    m_minUs = m_prevUs;
}
void TimedTask::TaskInit(void)
{
  m_prevUs = m_maxUs = m_minUs = m_avgUs = 0;
  ptr_task->TaskInit();
}
