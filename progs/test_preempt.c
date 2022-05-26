
/*
* Tests preemption
*/

#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

int thread3(void* arg)
{
 printf("thread%d\n", uthread_self());
 exit(0);

 return 3;
}

int thread2(void* arg)
{
 uthread_create(thread3, NULL);
 printf("thread%d\n", uthread_self());
 while (1) {
   //do nothing
 }

 return 2;
}

int thread1(void* arg)
{
 uthread_create(thread2, NULL);
 printf("thread%d\n", uthread_self());
 while (1) {
   //do nothing
 }

 return 1;
}

int main(void)
{
  uthread_create(thread1, NULL); //this will start timer
  while(1){
    //do nothing
  }

  return 0;
}
