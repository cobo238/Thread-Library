#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

struct queue { // node object
  queue_t next; // pointer to next node
  void *data; // pointer to data
};

queue_t queue_create(void)
{
  queue_t head = (queue_t) malloc(sizeof(struct queue)); // make new node
  if (head == NULL) { // if malloc failed, return NULL
    return NULL;
  }
  head -> next = NULL; // next node is NULL
  head -> data = head; // data points to tail (the node itself)
  return head; // return pointer to node
}

int queue_destroy(queue_t queue)
{
  /* Return error if queue DNE or is not empty */
  if (queue == NULL ||  queue -> next != NULL) {
    return -1;
  }
  free(queue); // deallocate the current node
  return 0; // success
}

int queue_enqueue(queue_t queue, void *data)
{ // append to tail
  if (queue == NULL || data == NULL) { // return error if variables = NULL
    return -1;
  }
  queue_t new_node = (queue_t) malloc(sizeof(struct queue)); // make new node
  if (new_node == NULL) { // if malloc failed, return error
    return -1;
  }
  new_node -> next = NULL; // new node points to NULL
  new_node -> data = data; // put data in new node
  queue_t tail = queue -> data;
  tail -> next = new_node; // append new_node to old tail
  queue -> data = new_node; // tail points to new_node
  return 0;
}

int queue_dequeue(queue_t queue, void **data)
{ // remove head
  if (queue == NULL || data == NULL) { // return error if variables = NULL
    return -1;
  }
  if (queue -> next == NULL) { // return error if array is empty
    return -1;
  }
  *data = queue -> next -> data;
  /* If only a single node exists, move tail */
  if (queue -> next == queue -> data) {
    queue -> data = queue; // tail = head
  } // else there is more than one node
  queue_t temp = queue -> next;
  queue -> next = queue -> next -> next; // head points to new node
  free(temp); // deallocate node
  return 0;
}

int queue_delete(queue_t queue, void *data)
{
  /* return error if variables = NULL or if only head node exists */
  if (queue == NULL || data == NULL || queue -> next == NULL) {
    return -1;
  }
  queue_t head_node = queue; // temp pointer to head
  queue_t current_node = queue -> next; // temp pointer to first data node
  while (current_node != NULL) { // iterate through all data nodes
    if (current_node -> data == data) { // if there is a match
      if (current_node -> next == NULL) { // if last node of queue
        head_node -> data = queue; // move tail to the previous node
      }
      /* previous node points past current node onto the next node */
      queue -> next = queue -> next -> next;
      free(current_node); // dealloc deleted node through temp pointer
      return 0;
    }
    queue = queue -> next; // if no match, increment previous node
    current_node = current_node -> next; // also increment current node
  }
  return -1; // return -1 if not found
}

int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data)
{
  if (queue == NULL)
    return -1;

  int function_return = 0;
  queue_t temp;
  temp = queue;
  temp = temp->next;  //skips over the head node
  /* if no errors, iterate through queue and call func */
  if (temp != NULL && func != NULL) {
    while (temp != NULL) {
      function_return = func(temp->data, arg);
      if( function_return == 1 ) {
        if (data != NULL)
          *data = temp->data;

        break;
      }
      temp = temp->next; //go to next node
    }
  }else {
    return -1;
  }

  return 0;
}

int queue_length(queue_t queue)
{
  int Q_length = 0;
  if (queue == NULL) // return error if no queue exists
    return -1;
  while (queue != NULL) { // iterate until end of queue is reached
    queue = queue->next;
    Q_length++; // add 1 for each node
  }
  return (Q_length - 1); // subtract by 1 to remove head node from count
}
