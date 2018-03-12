#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MinHeap.h"
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/time.h>
pthread_mutex_t heapMutex = PTHREAD_MUTEX_INITIALIZER;
MinHeap * heap;
sem_t  timerSemaphore;

typedef struct request{
  int timespan;
  char requestString[100];
  struct timespec absoluteTime;
}request;

static void timerHandler (){
    sem_post(&timerSemaphore);
}

static void start_timer(int first_quantum, int repeat_quantum) {
    struct itimerval interval;
    interval.it_interval.tv_sec = repeat_quantum;
    interval.it_interval.tv_usec = 0;
    interval.it_value.tv_sec = first_quantum;
    interval.it_value.tv_usec = 0;
    setitimer(ITIMER_REAL, &interval, 0);
}

void * readerThread (void *arg){

  struct timespec currentTime;
  int span = 1;
  int relativeSpan = 0;
  char str[100];

  while(scanf("%d %[^\n]", &span, str) > 0){

    if (span < 0){
      printf("PROGRAM EXITING\n");
      destroyMinHeap(heap);
      exit(0);
    }

    clock_gettime(CLOCK_REALTIME, &currentTime);
    request * event;
    event = malloc(sizeof(request));
    event->timespan = span;

    strcpy(event->requestString, str);

    event->absoluteTime.tv_sec = span + currentTime.tv_sec;

    pthread_mutex_lock(&heapMutex);

    addElement(heap, event);

    request * topEvent = (request*)getMin(heap);

    relativeSpan = topEvent->absoluteTime.tv_sec - currentTime.tv_sec;

    start_timer(0,0);
    start_timer(relativeSpan, 0);

    pthread_mutex_unlock(&heapMutex);
  }

  return NULL;
}

void * schedulerThread( void  * ignore){
  struct timespec currentTime;
  int  span = 0;
  for (;;){

    sem_wait(&timerSemaphore); //wait to expire

    clock_gettime(CLOCK_REALTIME, &currentTime); //get current time

    start_timer(0,0); // deactivate timer
    pthread_mutex_lock(&heapMutex);
    request * topEvent = (request*)getMin(heap);
    while ((topEvent->absoluteTime.tv_sec <= currentTime.tv_sec) && (heap->elemcount > 0)){
        //check all elements in heap
        printf("Request invoked: %d %s\n", topEvent->timespan, topEvent->requestString);
        removeMin(heap, &topEvent);
        clock_gettime(CLOCK_REALTIME, &currentTime);

        if (heap->elemcount > 0)
        {
          topEvent = getMin(heap);
        }
        else
        {
          break;
        }

    }

    if (heap->elemcount > 0){
      clock_gettime(CLOCK_REALTIME, &currentTime);
      span = topEvent->absoluteTime.tv_sec - currentTime.tv_sec;
      start_timer(span,0);
    }
    pthread_mutex_unlock(&heapMutex);
  }
  return NULL;
}

int comparator(const void * a, const void * b){ //have to change to const void *
  request * event1 = (request*) a;
  request * event2 = (request*) b;
  if (event1->absoluteTime.tv_sec != event2->absoluteTime.tv_sec){
    return event1->absoluteTime.tv_sec - event2->absoluteTime.tv_sec;
  }
  return event1->absoluteTime.tv_nsec - event2->absoluteTime.tv_nsec;
}

int main(){

  pthread_t rThread, scThread;

  sem_init(&timerSemaphore, 0, 0); //intialize semaphore
  heap = malloc(sizeof(MinHeap));
  initializeMinHeap(heap, (Comparator)comparator, sizeof(request)); //intialize MinHeap

  // set up SIGALARM
  struct sigaction action;
  action.sa_flags = SA_SIGINFO | SA_RESTART;
  action.sa_sigaction = timerHandler;
  sigemptyset (&action.sa_mask);
  sigaction(SIGALRM, &action, 0);
  printf("\n|--------------------------------------------------------|");
  printf("\n|To exit program enter a negative number with a character|");
  printf("\n| Example: (-1 e), (-2 exit)                             |" );
  printf("\n|--------------------------------------------------------|");
  printf("\n\nEnter a request: \n");
  if (pthread_create(&rThread, NULL, &readerThread, 0) < 0){
    printf("problem creating thread\n");
    exit(0);
  }

  if (pthread_create(&scThread, NULL, &schedulerThread, 0) < 0){
    printf("problem creating thread\n");
    exit(0);
  }
  pthread_exit(0);
}
