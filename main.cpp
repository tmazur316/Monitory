#include <cstdio>
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include "monitor.h"
#include "buffer_handler.cpp"
#include <semaphore.h>

elements n;
producer_handler handler;
sem_t mutex;

class buffer: Monitor
{
private:
    int *tab;
    int count;//number of elements in the buffer
    Condition empty;

    void print1(int number)
    {
      sem_wait(&mutex);
      std::ostringstream oss;
      oss<<"Kolejka "<<number<<": "<<"dodano element "<<tab[count -1]<<"| Stan kolejki: ";
      for(int i = 0; i < count; ++i)
        oss<<tab[i]<<" ";
      std::string s=oss.str();
      std::cout<<s<<std::endl;
      sem_post(&mutex);
    }

    void print2(int number, int erased)
    {
      sem_wait(&mutex);
      std::ostringstream oss;
      oss<<"Kolejka "<<number<<": "<<"usunieto element "<<erased<<"| Stan kolejki: ";
      for(int i = 0; i < count; ++i)
        oss<<tab[i]<<" ";
      std::string s=oss.str();
      std::cout<<s<<std::endl;
      sem_post(&mutex);
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

      if(count > 0)
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

buffer* array_of_buffers;

void* producer(void*)
{
  bool inserted = false;
  int number = std::rand() % 5;
  handler.producer_stop();
  while(true)
  {
    n.plus_count();
    for (int i = 0; i < 5; ++i)
    {
      inserted = array_of_buffers[number].insert_item(number + 1);
      if(inserted)
        break;
      number = (number + 1) % 5;
    }
    if(inserted)
    {
      handler.producer_resume();
      break;
    }
  }
}

void* consumer1(void*)
{
  array_of_buffers[0].erase_item(1);
  n.minus_count();
}

void* consumer2(void*)
{
  array_of_buffers[1].erase_item(2);
  n.minus_count();
}

void* consumer3(void*)
{
  array_of_buffers[2].erase_item(3);
  n.minus_count();
}

void* consumer4(void*)
{
  array_of_buffers[3].erase_item(4);
  n.minus_count();
}

void* consumer5(void*)
{
  array_of_buffers[4].erase_item(5);
  n.minus_count();
}

void perform_test1()
{
  while(true)
  {
    pthread_t new_thread;
    usleep(80000);

    pthread_create(&new_thread, NULL, producer, NULL);

    int number=std::rand()%5;
    if (number == 0) pthread_create(&new_thread, NULL, consumer1, NULL);
    else if (number == 1) pthread_create(&new_thread, NULL, consumer2, NULL);
    else if (number == 2) pthread_create(&new_thread, NULL, consumer3, NULL);
    else if (number == 3) pthread_create(&new_thread, NULL, consumer4, NULL);
    else if (number == 4 )pthread_create(&new_thread, NULL, consumer5, NULL);

  }
}

void perform_test2()
{
  while(true)
  {
    pthread_t new_thread;
    usleep(80000);

    for(int i = 0; i < 2; i++)
      pthread_create(&new_thread, NULL, producer, NULL);

    int number=std::rand()%5;
    if (number == 0) pthread_create(&new_thread, NULL, consumer1, NULL);
    else if (number == 1) pthread_create(&new_thread, NULL, consumer2, NULL);
    else if (number == 2) pthread_create(&new_thread, NULL, consumer3, NULL);
    else if (number == 3) pthread_create(&new_thread, NULL, consumer4, NULL);
    else if (number == 4) pthread_create(&new_thread, NULL, consumer5, NULL);
  }
}

void perform_test3()
{
  while(true)
  {
    pthread_t new_thread;
    usleep(80000);

    pthread_create(&new_thread, NULL, producer, NULL);
    int number;

    for (int i = 0; i < 2; ++i)
    {
      number=std::rand()%5;
      if (number == 0) pthread_create(&new_thread, NULL, consumer1, NULL);
      else if (number == 1) pthread_create(&new_thread, NULL, consumer2, NULL);
      else if (number == 2) pthread_create(&new_thread, NULL, consumer3, NULL);
      else if (number == 3) pthread_create(&new_thread, NULL, consumer4, NULL);
      else if (number == 4) pthread_create(&new_thread, NULL, consumer5, NULL);
    }
  }
}

int main()
{
  array_of_buffers= new buffer[5];
  char c;
  srand(time(NULL));
  sem_init(&mutex, 0, 1);

  std::cout<<"Wybierz test:"<<std::endl<<"1-test zwykly"<<std::endl<<"2-test z nadmiarowym producentem"<<std::endl<<"3-test z nadmiarowymi konsumentami"<<std::endl;
  std::cin>>c;
  if(c=='1') perform_test1();
  else if (c=='2') perform_test2();
  else if(c=='3') perform_test3();

  delete []array_of_buffers;
  sem_destroy(&mutex);
  return 0;
}