#ifndef _CLASS_DEFS_h
#define _CLASS_DEFS_h

#include <cstddef>
#include <vector>
using namespace std;


typedef enum EVENT_TYPE
{
  EVENT_MOTOR_DRV = 0,
  EVENT_IR = 1,
  EVENT_MQTT = 2,
  EVENT_WEB = 3,
  EVENT_SCHED = 4,
  EVENT_WIFI = 5
} EVENT_TYPE;

//--------- forward declarations ---------
class Task;
class Timer1ms;
class Timer1sec;
class Timer1min;
class TaskScheduler;


class TaskScheduler
{
  private:
    static vector<Task*> m_taskList;
    static vector<Timer1ms*> m_Timer1msList;
    static vector<Timer1sec*> m_Timer1secList;
    static vector<Timer1min*> m_Timer1minList;

  public:
    static void regTask(Task*);
    static void initTasks(void);
    static void runSchedOnce(void);

    static void regTimer1ms(Timer1ms*);
    static void runAllTimer1ms(void);

    static void regTimer1sec(Timer1sec*);
    static void runAllTimer1sec(void);

    static void regTimer1min(Timer1min*);
    static void runAllTimer1min(void);
};



class Task
{
  public:
    Task(void); //base constructor; registers itself to TaskScheduler
    virtual void TaskRun (void) = 0;
    virtual void TaskInit (void) = 0;
};

class TimedTask : public Task
{
  private:
    Task* ptr_task;
    unsigned long m_prevUs;
    unsigned long m_maxUs;
    unsigned long m_minUs;
    unsigned long m_avgUs;
  
  public:
    TimedTask(Task*);

    //inherited
    void TaskRun(void);
    void TaskInit(void);
};




class Timer1ms
{
  public:
    Timer1ms(void);
    virtual void Tick1ms (void) = 0;
};

class Timer1sec
{
  public:
    Timer1sec(void);
    virtual void Tick1sec (void) = 0;
};

class Timer1min
{
  public:
    Timer1min(void);
    virtual void Tick1min (void) = 0;
};



template <class T>
class Singleton
{
  public:  
    static T* getInstance(void)
    {
      if (!m_instance)
        m_instance = new T(); 
      return m_instance;
    }

  protected:
    Singleton() { };
    virtual ~Singleton() { };

  private:
    static T* m_instance;
};

//implicit instantiation
template<class T>
T* Singleton<T>::m_instance = NULL;




template <class T>
class Observer
{
  public:
    Observer() {}
    virtual ~Observer() {}
    virtual void onEvent(T) = 0;
};

template <class T>
class Subject
{
  public:
    Subject() {}
    virtual ~Subject() {}
    void attach (Observer<T> *observer)
    {
      m_observers.push_back(observer);
    }
    void notify (T param)
    {
      for (auto it=m_observers.begin(); it!=m_observers.end(); it++) 
        (*it)->onEvent(param);
    }
  private:
    std::vector<Observer<T>*> m_observers;
};



#endif

