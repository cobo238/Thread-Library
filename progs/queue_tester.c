#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mcheck.h>
#include <malloc.h>
#include <signal.h>

#include "queue.h"

void test_create(void)
{ // tests the creation of an empty queue
    queue_t q; // create a new queue variable type

    q = queue_create(); // initialize a new empty queue
    assert(q != NULL); // make sure that a new queue was created
    assert(mprobe(q) == 0); // test the memory integrity of the queue

    queue_destroy(q); // delete an empty queue
    q = NULL; // clear the address of the queue
}

void test_destroy(void)
{ // tests the deletion of an empty queue
  queue_t q; // create a new queue variable type

  q = queue_create(); // initialize a new empty queue
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  q = NULL; // clear the address of the queue
  assert(result == 0); // test the error code of queue_destroy
  assert(q == NULL); // make sure that the address was cleared
}

void test_destroy_delete(void)
{ // tests the deletion of an empty queue where variables were deleted
  queue_t q; // create a new queue variable type
  int data = 85; // variable to be added to queue

  q = queue_create(); // initialize a new empty queue
  queue_enqueue(q, &data); // add data to the beginning of the queue
  queue_delete(q, &data); // remove data from the end of the queue
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
  q = NULL; // clear the address of the queue
  assert(q == NULL); // make sure that the address was cleared
}

void test_destroy_dequeue(void)
{ // tests the deletion of an empty queue where variables were dequeued
  queue_t q; // create a new queue variable type
  int data = 85, *ptr; // variable to be added to queue

  q = queue_create(); // initialize a new empty queue
  queue_enqueue(q, &data); // add data to the beginning of the queue
  queue_dequeue(q, (void**)&ptr); // remove data and put in ptr
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  assert(&data == ptr); // make sure data added = data returned
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
  q = NULL; // clear the address of the queue
  assert(q == NULL); // make sure that the address was cleared
}

void test_destroy_dequeue_delete(void)
{ // tests the deletion of a queue with both dequeued and deleted variables
  queue_t q; // create a new queue variable type
  int data[2] = {1, 85}; // variables to be added to queue
  int *ptr;

  q = queue_create(); // initialize a new empty queue
  queue_enqueue(q, &data[0]); // add data to the beginning of the queue
  queue_enqueue(q, &data[1]);
  queue_delete(q, &data[0]); // remove data from the end of the queue
  queue_dequeue(q, (void**)&ptr); // remove data and put in ptr
  assert(&data[1] == ptr); // make sure data added = data returned
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
  q = NULL; // clear the address of the queue
  assert(q == NULL); // make sure that the address was cleared
}

void test_queue_simple(void)
{ // tests a single enqueue and dequeue
    queue_t q; // create a new queue variable type
    int data = 3, *ptr;

    q = queue_create(); // initialize a new empty queue
    queue_enqueue(q, &data);
    queue_dequeue(q, (void**)&ptr);
    assert(ptr == &data);
    assert(mprobe(q) == 0); // test the memory integrity of the queue
    int result = queue_destroy(q); // delete an empty queue
    assert(result == 0); // test the error code of queue_destroy
}

void test_double_node(void)
{ // tests a double enqueue and dequeue
    queue_t q; // create a new queue variable type
    int data[2] = {3, 7};
    int *ptr1, *ptr2;

    q = queue_create(); // initialize a new empty queue
    queue_enqueue(q, &data[0]);
    queue_enqueue(q, &data[1]);
    queue_dequeue(q, (void**)&ptr1);
    queue_dequeue(q, (void**)&ptr2);
    assert(ptr1 == &data[0]);
    assert(ptr2 == &data[1]);
    assert(mprobe(q) == 0); // test the memory integrity of the queue
    int result = queue_destroy(q); // delete an empty queue
    assert(result == 0); // test the error code of queue_destroy
}

void test_single_delete(void)
{ // tests a single delete in the middle with dequeue
  queue_t q; // create a new queue variable type
  int data[3] = {62, 12 ,49};
  int *ptr1, *ptr2;

  q = queue_create(); // initialize a new empty queue
  queue_enqueue(q, &data[0]); // add 3
  queue_enqueue(q, &data[1]);
  queue_enqueue(q, &data[2]);

  queue_delete(q, &data[0]); // delete 1

  queue_dequeue(q, (void**)&ptr1); // remove 2
  queue_dequeue(q, (void**)&ptr2);
  assert(ptr1 == &data[1]);
  assert(ptr2 == &data[2]);
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
}

void test_end_delete(void)
{ // tests deleting an end node
  queue_t q; // create a new queue variable type
  int data[3] = {10, 19 ,33};
  int *ptr1, *ptr2;

  q = queue_create(); // initialize a new empty queue
  queue_enqueue(q, &data[0]); // add 3
  queue_enqueue(q, &data[1]);
  queue_enqueue(q, &data[2]);

  queue_delete(q, &data[2]); // delete 1

  queue_dequeue(q, (void**)&ptr1); // remove 2
  queue_dequeue(q, (void**)&ptr2);
  assert(ptr1 == &data[0]);
  assert(ptr2 == &data[1]);
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
}

void test_double_delete(void)
{ // tests deleting 2 nodes
  queue_t q; // create a new queue variable type
  char data[4] = {'a', 'b', 'c', 'd'};
  char *ptr1, *ptr2;

  q = queue_create(); // initialize a new empty queue
  queue_enqueue(q, &data[0]); // add 4
  queue_enqueue(q, &data[1]);
  queue_enqueue(q, &data[2]);
  queue_enqueue(q, &data[3]);

  queue_delete(q, &data[0]); // delete 2
  queue_delete(q, &data[2]);

  queue_dequeue(q, (void**)&ptr1); // remove 2
  queue_dequeue(q, (void**)&ptr2);
  assert(ptr1 == &data[1]);
  assert(ptr2 == &data[3]);
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
}

void test_length_zero(void)
{ // tests the length of a queue with no data
    queue_t q; // create a new queue variable type

    q = queue_create(); // initialize a new empty queue
    assert(q != NULL); // make sure that a new queue was created
    int length = queue_length(q);
    assert(length == 0);
    assert(mprobe(q) == 0); // test the memory integrity of the queue
    int result = queue_destroy(q); // delete an empty queue
    assert(result == 0); // test the error code of queue_destroy
}

void test_length_two(void)
{ // tests the length of a queue after adding 1, then 2 nodes
  queue_t q; // create a new queue variable type
  char data[2] = {'a', 'b'};

  q = queue_create(); // initialize a new empty queue
  int length = queue_length(q);
  assert(length == 0);
  queue_enqueue(q, &data[0]); // add

  length = queue_length(q);
  assert(length == 1);
  queue_enqueue(q, &data[1]); // add

  length = queue_length(q);
  assert(length == 2);
  queue_delete(q, &data[1]); // delete

  length = queue_length(q);
  assert(length == 1);
  queue_delete(q, &data[0]); // delete

  length = queue_length(q);
  assert(length == 0);
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
}

/* Callback function that increments items by a certain value */
static int inc_item(void *data, void *arg)
{
    /* Increment single piece of data by one */
    int *a = (int*)data;
    int inc = (int)(long)arg;
    *a += inc;

    return 0;
}

/* Callback function that finds a certain item according to its value */
static int find_item(void *data, void *arg)
{
    /* Return 1 if piece of data is a match */
    int *a = (int*)data;
    int match = (int)(long)arg;
    if (*a == match)
        return 1;
    /* Otherwise return 0 */
    return 0;
}

void test_iterator(void)
{
    queue_t q;
    int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int unsigned i;
    int *ptr;
    /* Initialize the queue and enqueue items */
    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);
    /* Add value '1' to every item of the queue */
    queue_iterate(q, inc_item, (void*)1, NULL);
    assert(data[0] == 2);
    assert(data[9] == 11);
    /* Find and get the item which is equal to value '5' */
    ptr = NULL;
    queue_iterate(q, find_item, (void*)5, (void**)&ptr);
    assert(ptr != NULL);
    assert(*ptr == 5);
    assert(ptr == &data[3]);
}

void test_no_func_iterator(void)
{
  queue_t q;
  int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int unsigned i;
  int result;

  /* Initialize the queue and enqueue items */
  q = queue_create();
  for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
      queue_enqueue(q, &data[i]);

  result = queue_iterate(q, NULL, (void*)1, NULL);
  assert(result == -1);
}

void test_q_null_iterator(void)
{
  queue_t q;
  int data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int unsigned i;
  int result;

  /* Initialize the queue and enqueue items */
  q = NULL; //make queue NULL
  for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
      queue_enqueue(q, &data[i]);

  result = queue_iterate(q, find_item , (void*)1, NULL);
  assert(result == -1);

}

void test_100_delete(void)
{ // tests enqueuing and deleting 100 elements
  queue_t q; // create a new queue variable type
  int data[100]; // create 100 elements of data
  for (int i = 0; i < 100; i++) {
    data[i] = i;
  }
  /* Enqueue 100 elements into the queue */
  q = queue_create(); // initialize a new empty queue
  int length = queue_length(q); // find length of empty queue
  assert(length == 0);
  for (int i = 0; i < 100; i++) {
    queue_enqueue(q, &data[i]);
    length = queue_length(q); // check length of queue for each enqueue
    assert(length == i + 1);
  }
  /* Delete 100 elements out of the queue */
  for (int i = 99; i >= 0; i--) {
    queue_delete(q, &data[i]);
    length = queue_length(q); // check length of queue for each delete
    assert(length == i);
  }
  /* Delete queue structure */
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
}

void test_1000_delete(void)
{ // tests enqueuing and deleting 1000 elements
  queue_t q; // create a new queue variable type
  int data[1000]; // create 100 elements of data
  for (int i = 0; i < 1000; i++) {
    data[i] = i;
  }
  /* Enqueue 1000 elements into the queue */
  q = queue_create(); // initialize a new empty queue
  int length = queue_length(q); // find length of empty queue
  assert(length == 0);
  for (int i = 0; i < 1000; i++) {
    queue_enqueue(q, &data[i]);
    length = queue_length(q); // check length of queue for each enqueue
    assert(length == i + 1);
  }
  /* Delete 1000 elements out of the queue */
  for (int i = 999; i >= 0; i--) {
    queue_delete(q, &data[i]);
    length = queue_length(q); // check length of queue for each dequeue
    assert(length == i);
  }
  /* Delete queue structure */
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
}

void test_100_dequeue(void)
{ // tests enqueuing and dequeuing 100 elements
  queue_t q; // create a new queue variable type
  int data[100]; // create 100 elements of data
  int* ptr[100];
  for (int i = 0; i < 100; i++) {
    data[i] = i;
  }
  /* Enqueue 100 elements into the queue */
  q = queue_create(); // initialize a new empty queue
  int length = queue_length(q); // find length of empty queue
  assert(length == 0);
  for (int i = 0; i < 100; i++) {
    queue_enqueue(q, &data[i]);
    length = queue_length(q); // check length of queue for each enqueue
    assert(length == i + 1);
  }
  /* Dequeue 100 elements out of the queue */
  for (int i = 0; i < 100; i++) {
    queue_dequeue(q, (void**)&ptr[i]); // remove data and put in ptr
    assert(ptr[i] == &data[i]);
    assert(*ptr[i] == data[i]);
  }
  /* Delete queue structure */
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
}

void test_1000_dequeue(void)
{ // tests enqueuing and dequeuing 1000 elements
  queue_t q; // create a new queue variable type
  int data[1000]; // create 100 elements of data
  int* ptr[1000];
  for (int i = 0; i < 1000; i++) {
    data[i] = i;
  }
  /* Enqueue 1000 elements into the queue */
  q = queue_create(); // initialize a new empty queue
  int length = queue_length(q); // find length of empty queue
  assert(length == 0);
  for (int i = 0; i < 1000; i++) {
    queue_enqueue(q, &data[i]);
    length = queue_length(q); // check length of queue for each enqueue
    assert(length == i + 1);
  }
  /* Dequeue 1000 elements out of the queue */
  for (int i = 0; i < 1000; i++) {
    queue_dequeue(q, (void**)&ptr[i]); // remove data and put in ptr
    assert(ptr[i] == &data[i]);
    assert(*ptr[i] == data[i]);
  }
  /* Delete queue structure */
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
}

void test_repeated_use(void)
{ // tests enqueuing, dequeuing, and deleting on the same array 100 times
  queue_t q; // create a new queue variable type
  int data[100]; // create 100 elements of data
  int* ptr[100];
  for (int i = 0; i < 100; i++) {
    data[i] = i;
  }
  /* Enqueue 100 elements into the queue */
  q = queue_create(); // initialize a new empty queue
  int length = queue_length(q); // find length of empty queue
  assert(length == 0);
  for (int i = 0; i < 99; i++) {
    /* Enqueue and Dequeue a single item */
    queue_enqueue(q, &data[i]);
    queue_dequeue(q, (void**)&ptr[i]); // remove data and put in ptr
    assert(ptr[i] == &data[i]);
    assert(*ptr[i] == data[i]);
    /* Enqueue 2 items, Dequeue 1 item */
    queue_enqueue(q, &data[i]);
    queue_enqueue(q, &data[i + 1]);
    queue_dequeue(q, (void**)&ptr[i]); // remove data and put in ptr
    assert(ptr[i] == &data[i]);
    assert(*ptr[i] == data[i]);
    /* Enqueue 1 item, Dequeue 1 item, Delete 1 item */
    queue_enqueue(q, &data[i]);
    queue_delete(q, &data[i]);
    queue_dequeue(q, (void**)&ptr[i + 1]); // remove data and put in ptr
    assert(ptr[i + 1] == &data[i + 1]);
    assert(*ptr[i + 1] == data[i + 1]);
    length = queue_length(q); // check length of queue for each enqueue
    assert(length == 0);
  }
  /* Delete queue structure */
  assert(mprobe(q) == 0); // test the memory integrity of the queue
  int result = queue_destroy(q); // delete an empty queue
  assert(result == 0); // test the error code of queue_destroy
}

int main(void) {
  assert(mcheck(NULL) == 0); // required for mprobe()
  mallopt(M_CHECK_ACTION, 2); // returns an error if a double free occurs

  /* run each queue test once */
  test_create();
  test_destroy();
  test_destroy_delete();
  test_destroy_dequeue();
  test_destroy_dequeue_delete();
  test_queue_simple();
  test_double_node();
  test_double_delete();
  test_single_delete();
  test_end_delete();
  test_length_zero();
  test_length_two();
  test_iterator();
  test_no_func_iterator();
  test_q_null_iterator();
  test_100_delete();
  test_1000_delete();
  test_100_dequeue();
  test_1000_dequeue();
  test_repeated_use();

  return 0;
}
