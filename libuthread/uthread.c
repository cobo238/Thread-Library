#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "context.h"
#include "preempt.h"
#include "queue.h"
#include "uthread.h"

static int new_thread_init(uthread_t TID, uthread_func_t func, void *arg);

static int uthread_init();

static int TID_max(void *data, void *arg);

static uthread_t max3(uthread_t v1, uthread_t v2, uthread_t v3);

static int get_data(void *data, void *arg);

static int get_data_TID(void *data, void *arg);

static int get_data_TID_join(void *data, void *arg);

static void collect_thread(uthread_t TID);

typedef struct thread_data thread_data;

/* Stores the data of each thread, including context. */
struct thread_data {
  uthread_t TID; // TID of the thread
  uthread_ctx_t context; // context of the thread
  void* stack_pointer; // pointer to the top of the thread stack
  int* return_value; // pointer to return value of join function
  int TID_join; // TID of thread to join
};

/* Stores the data of all ready or running threads. */
queue_t ready_q = NULL;

/*stores the data of all blocked threads*/
queue_t block_q = NULL;

/*stores the data of all zombie threads*/
queue_t zombie_q = NULL;

/* Initializes a new thread and places it in the ready_q */
static int new_thread_init(uthread_t TID, uthread_func_t func, void *arg)
{
  /* Initialize a new thread struct */
  void* stack_pointer = uthread_ctx_alloc_stack();
  if (stack_pointer == NULL)
    return -1; // return error if stack allocation fails
  uthread_ctx_t new_context;
  if (uthread_ctx_init(&new_context, stack_pointer, func, arg) != 0)
    return -1; // return error if main initialization fails
  thread_data* new_thread = (thread_data*) malloc(sizeof(thread_data));
  new_thread->TID = TID;
  new_thread->context = new_context;
  new_thread->stack_pointer = stack_pointer;
  new_thread->return_value = NULL;
  new_thread->TID_join = 0;
  preempt_disable();
  if (queue_enqueue(ready_q, (void*)new_thread) == -1) // enqueue thread
    return -1; // return error if enqueuing fails
  preempt_enable();

  return 0; // return 0 if no errors
}

/* Initialize queues and main thread */
static int uthread_init()
{
  /* ready_q stores threads that are ready to be run
     block_q stores queues that are blocked until they join another thread
     zombie_q stores threads that exited but have not been collected */
  ready_q = queue_create();
  block_q = queue_create();
  zombie_q = queue_create();
  if (ready_q == NULL || block_q == NULL || zombie_q == NULL)
    return -1; // return error if queue creation fails
  preempt_start(); // starts timer and setups signal handler
  if (new_thread_init(0, 0, 0) == -1)
    return -1; // return error if thread init failed

  return 0; // return 0 if no errors
}

/* Callback function that returns largest TID being used */
static int TID_max(void *data, void *arg)
{
  /* Return 1 if piece of data is a match */
  thread_data* a = (thread_data*)data;
  uthread_t TID_temp = a->TID; // new TID
  uthread_t *arg_temp = (uthread_t*)arg; // stored TID
  if (TID_temp > *arg_temp) // if new TID is larger, store it
      *arg_temp = TID_temp;

  return 0; // return 0 to keep iterating through queue
}

/* Find the largest of 3 values */
static uthread_t max3(uthread_t v1, uthread_t v2, uthread_t v3)
{
  /* Finds max between v1 and v2 */
  uthread_t max = 0;
  if (v1 > v2) {
    max = v1;
  } else {
    max = v2;
  }
  /* Finds max between max(v1, v2) and v3 */
  if (v3 > max)
    max = v3;

  return max; // return largest value
}

/* Callback function that provides thread info from queue.
   arg = location of data node in queue */
static int get_data(void *data, void *arg)
{
  if (data == NULL) // variable should not be used
    return -1;
  if ((*(int*)arg) == 1) { // if arg = 1 then get current node
    return 1;
  } else {
    (*(int*)arg)--; // else decrement arg for next loop
  }

  return 0; // return 0 to continue through queue
}

/* Callback function that finds the position of a thread in ready_q.
   Will set arg equal to position (arg should be set to 0 beforehand) */
static int get_data_TID(void *data, void *arg)
{
  /* Return 1 if TID is a match */
  thread_data* a = (thread_data*)data;
  uthread_t TID_temp = a->TID;
  uthread_t *arg_temp = (uthread_t*)arg;
  /* Increment the position in the queue */
  if (TID_temp == *arg_temp) { // if TID is found, return
      return 1;
  }

  return 0; // return 0 to continue through queue
}

/* Callback function that returns the data of a thread with a given TID_join */
static int get_data_TID_join(void *data, void *arg) {
  /* Return 1 if TID_join is a match */
  thread_data* a = (thread_data*)data;
  uthread_t TID_join_temp = a->TID_join; // TID_join in next node in queue
  uthread_t *arg_temp = (uthread_t*)arg;
  if (TID_join_temp == *arg_temp) // if TID_join is found, return
      return 1;

  return 0; // return 0 to continue through queue
}

/* Deallocates a zombie thread given a TID */
static void collect_thread(uthread_t TID)
{
  preempt_disable();
  thread_data* data_current; // data to collect
  queue_iterate(zombie_q, get_data_TID, &TID , (void**)&data_current);
  uthread_ctx_destroy_stack(data_current->stack_pointer); // clear stack
  queue_delete(zombie_q, (void *)data_current);
  free(data_current); // free pointer
  preempt_enable();
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* Initialize thread queues and main thread if first time running */
  if (ready_q == NULL && uthread_init() == -1)
    return -1; // return error if initialization failed

  /* Find biggest queue value */
  uthread_t TID_biggest = 0, TID_rmax = 0, TID_bmax = 0, TID_zmax = 0;
  queue_iterate(ready_q, TID_max, (void*)&TID_rmax, NULL);
  queue_iterate(block_q, TID_max, (void*)&TID_bmax, NULL);
  queue_iterate(zombie_q, TID_max, (void*)&TID_zmax, NULL);
  // new queue TID is equal to largest existing TID + 1
  TID_biggest = max3(TID_rmax, TID_bmax, TID_zmax) + 1;

  /* Initialize the new thread */
  if (new_thread_init(TID_biggest, func, arg) == -1)
    return -1; // return error if thread init failed

  return (TID_biggest); // return TID of new thread
}

uthread_t uthread_self(void)
{
	/* Returns the TID of the thread that is running */
  int i = 1;
  thread_data* data_current;
  // gets the thread context from first thread in queue
  queue_iterate(ready_q, get_data, (void*)&i , (void**)&data_current);
  return data_current->TID;
}

void uthread_yield(void)
{
  preempt_disable();
  /* Yield if another thread is ready to run */
  if (queue_length(ready_q) > 1) {
    thread_data* data_current; // current thread
    thread_data* data_oldest; // next thread in queue
    // removes current running thread from ready queue
    queue_dequeue(ready_q, (void**)&data_current);
    int i = 1;
    // get first thread in queue
    queue_iterate(ready_q, get_data, (void*)&i , (void**)&data_oldest);
    // move running thread to end of ready queue, putting it in its ready state
    queue_enqueue(ready_q, (void*)data_current);
    // makes next thread in queue the running thread
    uthread_ctx_switch(&(data_current->context), &(data_oldest->context));
  }
  preempt_enable();
}

void uthread_exit(int retval)
{
	/* Gets data from exiting (current) node */
  uthread_t TID_exit = uthread_self(); // TID of exiting thread
  thread_data* data_current;

  /* Turn exiting node into zombie */
  preempt_disable();
  queue_dequeue(ready_q, (void**)&data_current); // remove from ready queue
  queue_enqueue(zombie_q, (void*)data_current); // add to zombie queue
  preempt_enable();

  /* Check if a thread to join exists */
  thread_data* data_parent = NULL;
  queue_iterate(block_q, get_data_TID_join, &TID_exit, (void**)&data_parent);
  if (data_parent != NULL) { // a thread to join exists
    if (data_parent->return_value != NULL) // if return variable is provided
      *(data_parent->return_value) = retval; // deref and set equal to retval

    /* Change state of parent from blocked to ready */
    preempt_disable();
    queue_delete(block_q, (void*) data_parent); // Unblock parent
    // Set TID_join value back to 0 because parent is no longer joining
    data_parent->TID_join = 0;
    queue_enqueue(ready_q, (void*)data_parent);
    preempt_enable();
    collect_thread(TID_exit); // collect exiting thread
  }

  /* If queue is not empty, switch to another node */
  if (queue_length(ready_q) > 0) {
    thread_data* data_oldest = NULL; // next thread in queue
    int i = 1;
    queue_iterate(ready_q, get_data, &i , (void**)&data_oldest);
    uthread_ctx_switch(&(data_current->context), &(data_oldest->context));
  }
}

int uthread_join(uthread_t tid, int *retval)
{
  int tid_parent = uthread_self(); // get tid of parent
	/* Check for error cases */
  if (tid == tid_parent || tid == 0) // check for TID errors
    return -1;

  /* Checks if TID is already being joined */
  thread_data* data_current = NULL;
  queue_iterate(block_q, get_data_TID_join, &tid, (void**)&data_current);
  if (data_current != NULL) // if join thread exists, return error
    return -1;

  /* Checks if child is already dead */
  queue_iterate(zombie_q, get_data_TID, &tid, NULL);
  if (data_current != NULL) {
    collect_thread(tid); // if already dead, collect and return
    return 0;
  }

  /* Checks if thread to join exists in ready queue */
  queue_iterate(ready_q, get_data_TID, (void*)&tid, (void**)&data_current);
  if (data_current == NULL)
    return -1;

  /* Sets the parent node to blocked */
  int i = 1;
  // grabs parent data from the first element in queue
  queue_iterate(ready_q, get_data, (void*)&i , (void**)&data_current);
  // set return value pointer for thread to be joined
  data_current->return_value = retval;
  // sets the parent join status equal to the tid of child
  data_current->TID_join = tid;
  preempt_disable();
  queue_dequeue(ready_q, (void**)&data_current); // remove parent from ready
  queue_enqueue(block_q, (void*)data_current); // add parent to blocked
  preempt_enable();
  thread_data* data_oldest;

  /* Switch to next ready thread after blocking parent */
  i = 1;
  queue_iterate(ready_q, get_data, (void*)&i , (void**)&data_oldest);
  uthread_ctx_switch(&(data_current->context), &(data_oldest->context));

  return 0; // return back to code of caller
}
