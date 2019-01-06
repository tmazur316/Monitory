#include "monitor.h"
#include <iostream>
#include <sstream>
#include <string>

class buffer: Monitor
{
  private:
    int *tab;
    int count;//number of elements in the buffer
    Condition empty;

    void print1(int number)
    {
      std::ostringstream oss;
      oss<<"Kolejka "<<number<<": "<<"dodano element "<<tab[count -1]<<"| Stan kolejki: ";
      for(int i = 0; i < count; ++i)
        oss<<tab[i]<<" ";
      oss<<std::endl;
      std::string s=oss.str();
      std::cout<<s<<std::endl;
    }

    void print2(int number, int erased)
    {
      std::ostringstream oss;
      oss<<"Kolejka "<<number<<": "<<"usunieto element "<<erased<<"| Stan kolejki: ";
      for(int i = 0; i < count; ++i)
        oss<<tab[i]<<" ";
      oss<<std::endl;
      std::string s=oss.str();
      std::cout<<s<<std::endl;
    }

  public:
    buffer(): Monitor(), count(0)
    {
      tab=new int[10];
    }

    ~buffer()
    {
      delete [] tab;
    }

    bool insert_item(int number)
    {
      enter();

      if(count==10)
      {
        leave();
        return false;
      }

      tab[count] = std::rand()%100;
      ++count;

      print1(number);

      if(count == 1)
        signal(empty);

      leave();
      return true;
    }

    void erase_item(int number)
    {
      enter();
      if(count == 0)
        wait(empty);

      int erased = tab[0];
      for(int i = 0; i < count-1; i++)
        tab[i]=tab[i+1];
      --count;

      print2(number, erased);

      leave();
    }
};

class producer_handler: Monitor
{
  private:
    bool stop;
    Condition to_stop;
    int count;

    void change_stop()
    {
      if(stop)
        stop = false;
      else stop = true;
    }

  public:
    producer_handler(): Monitor()
    {
      stop = false;
      count = 0;
    }

    void producer_stop()
    {
      enter();
      if(stop)
        wait(to_stop);
      change_stop();
      leave();
    }

    void producer_resume()
    {
      enter();
      change_stop();
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

    void stop_producer()
    {
      enter();
      if(count == 50)
        wait(stop);
      leave();
    }
};