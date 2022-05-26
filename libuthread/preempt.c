#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "preempt.h"
#include "uthread.h"

/*
* Frequency of preemption
* 100Hz is 100 times per second
*/
#define HZ 100
sigset_t set;

static void alarm_handler(int signum);

static void alarm_handler(int signum) { //signal handler function
 if (signum == SIGVTALRM)
   uthread_yield(); //forces a yield
}

void preempt_disable(void)
{
  sigprocmask(SIG_BLOCK, &set, NULL); //blocks SIGVTALRM
}

void preempt_enable(void)
{
  sigprocmask(SIG_UNBLOCK, &set, NULL); //unblocks SIGVTALRM
}

void preempt_start(void)
{
 sigemptyset(&set); //make set of signals empty
 sigaddset(&set,SIGVTALRM); //add SIGVTALRM to signal set

 /*Setup for timer handler*/
 struct sigaction handle_specs; //contains specifications for handling
 memset(&handle_specs, 0, sizeof(handle_specs)); //sets all specifications to 0
 handle_specs.sa_handler = &alarm_handler; //function handler to call

 sigaction(SIGVTALRM, &handle_specs, NULL); // sets up signal handler

 /*Configuring Timer*/
 // 100Hz -> T = 10ms = 10000us
 struct itimerval timer_settings; //contains settings for timer

 //the first alarm turns on after 10000us
 timer_settings.it_value.tv_sec = 0;
 timer_settings.it_value.tv_usec = 10000;

 //the period is set to 10000us
 timer_settings.it_interval.tv_sec = 0;
 timer_settings.it_interval.tv_usec = 10000;

 if (setitimer(ITIMER_VIRTUAL, &timer_settings, NULL) != 0) //starting the timer
   printf("timer setup error\n");

}
