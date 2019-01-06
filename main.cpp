#include <cstdio>
#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include "monitor.h"
#include "bufor.cpp"

buffer* array_of_buffers;
elements n;

void* producer(void* handler_arg)
{
  producer_handler *handler;
  handler = (producer_handler *) handler_arg;

  bool inserted = false;
  int number = std::rand() % 5;
  int i=number;

  handler->producer_stop();

  while(true)
  {
    for(int i = 0; i<5; ++i)
    {
      inserted = array_of_buffers[number].insert_item(number + 1);
      if (inserted) break;
      number = (number+1) % 5;
    }
    if(inserted) break;
    else n.stop_producer();
  }
  n.plus_count();
  handler->producer_resume();
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

void* consumer4(void* handler_arg)
{
  array_of_buffers[3].erase_item(4);
  n.minus_count();
}

void* consumer5(void* handler_arg)
{
  array_of_buffers[4].erase_item(5);
  n.minus_count();
}

void perform_test1()
{
  producer_handler handler;
  while(true)
  {
    pthread_t new_thread;
    int number=rand()%5;
    usleep(100000);

    pthread_create(&new_thread, NULL, producer, (void *) &handler);

    if (number == 0) pthread_create(&new_thread, NULL, consumer1, NULL);
    else if (number == 1) pthread_create(&new_thread, NULL, consumer2, NULL);
    else if (number == 2) pthread_create(&new_thread, NULL, consumer3, NULL);
    else if (number == 3) pthread_create(&new_thread, NULL, consumer4, NULL);
    else if (number == 4) pthread_create(&new_thread, NULL, consumer5, NULL);
    else break;
  }
}

void perform_test2()
{
  producer_handler handler;
  while(true)
  {
    pthread_t new_thread;
    int number=rand()%5;
    usleep(100000);

    for(int i = 0; i < 2; i++)
      pthread_create(&new_thread, NULL, producer, (void *) &handler);

    if (number == 0) pthread_create(&new_thread, NULL, consumer1, NULL);
    else if (number == 1) pthread_create(&new_thread, NULL, consumer2, NULL);
    else if (number == 2) pthread_create(&new_thread, NULL, consumer3, NULL);
    else if (number == 3) pthread_create(&new_thread, NULL, consumer4, NULL);
    else if (number == 4) pthread_create(&new_thread, NULL, consumer5, NULL);
    else break;
  }
}

void perform_test3()
{
  producer_handler handler;
  while(true)
  {
    pthread_t new_thread;
    int number = rand() % 5;
    usleep(100000);

    pthread_create(&new_thread, NULL, producer, (void *) &handler);

    if (number == 0)
    {
      for(int i = 0; i < 2; i++)
        pthread_create(&new_thread, NULL, consumer1, NULL);
    }
    else if (number == 1)
    {
      for(int i = 0; i < 2; i++)
        pthread_create(&new_thread, NULL, consumer2, NULL);
    }
    else if (number == 2)
    {
      for(int i = 0; i < 2; i++)
        pthread_create(&new_thread, NULL, consumer3, NULL);
    }
    else if (number == 3)
    {
      for(int i = 0; i < 2; i++)
        pthread_create(&new_thread, NULL, consumer4, NULL);
    }
    else if (number == 4)
    {
      for(int i = 0; i < 2; i++)
        pthread_create(&new_thread, NULL, consumer5, NULL);
    }
    else break;
  }
}

int main()
{
  array_of_buffers= new buffer[5];
  char c;
  srand(time(NULL));

  std::cout<<"Wybierz test:"<<std::endl<<"1-test zwykly"<<std::endl<<"2-test z nadmiarowym producentem"<<std::endl<<"3-test z nadmiarowymi konsumentami"<<std::endl;
  std::cin>>c;
  if(c=='1') perform_test1();
  else if (c=='2') perform_test2();
  else if(c=='3') perform_test3();

  delete []array_of_buffers;
  return 0;
}