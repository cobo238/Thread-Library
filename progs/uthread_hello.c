/*
 * Simple hello world test
 *
 * Tests the creation of a single thread and its successful return.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <uthread.h>

int hello(void* arg)
{
	printf("Hello world!\n");
	return *(int *)arg;
}

int main(void)
{
	uthread_t tid;
  int send_val = 3;
	int ret_val = 0;
	tid = uthread_create(hello, &send_val);
  assert(ret_val == 0);
  uthread_join(tid, &ret_val);
  assert(ret_val == 3);
	return 0;
}
