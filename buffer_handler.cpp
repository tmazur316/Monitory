#include "monitor.h"
#include <iostream>
#include <sstream>
#include <string>

class producer_handler: Monitor
{
  private:
    bool stop;
    Condition to_stop;

  public:
    producer_handler(): Monitor()
    {
      stop = true;
    }

    void producer_stop()
    {
      enter();
      if(!stop)
        wait(to_stop);
      stop = false;
      leave();
    }

    void producer_resume()
    {
      enter();
      stop = true;
      signal(to_stop);
      leave();
    }
};

class elements: Monitor
{
  private:
    int count;
    Condition stop;

  public:
    elements(): Monitor()
    {
      count=0;
    }

    void plus_count()
    {
      enter();
      if(count == 50)
        wait(stop);
      count++;
      leave();
    }

    void minus_count()
    {
      enter();
      count--;
      if(count == 49)
        signal(stop);
      leave();
    }
};