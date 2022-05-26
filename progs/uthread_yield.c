/*
 * Thread join test
 *
 * Tests the joining of multiple threads. The way the printing,
 * thread creation, joining and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <uthread.h>

int thread3(void* arg)
{
	uthread_yield();
	printf("thread%d\n", uthread_self());
	return *(int *)arg;
}

int thread2(void* arg)
{
	int ret_val = 0;
	int send_val = 7;
	int tid;
	tid = uthread_create(thread3, &send_val);
	assert(uthread_join(0, &ret_val) == -1);
	assert(uthread_join(uthread_self(), &ret_val) == -1);
	assert(uthread_join(100, &ret_val) == -1);
	printf("thread%d\n", uthread_self());
	uthread_yield();
	assert(uthread_self() == 2);
	uthread_join(tid, &ret_val);
	assert(uthread_join(tid, &ret_val) == -1);
	printf("return value%d\n", ret_val);
	return 5;
}

int thread1(void* arg)
{
	uthread_create(thread2, NULL);
	printf("thread%d\n", uthread_self());
	uthread_yield();
	assert(uthread_self() == 1);
	uthread_yield();
	return 4;
}

int main(void)
{
	int ret_val = 0;
	uthread_join(uthread_create(thread1, NULL), &ret_val);
	assert(ret_val == 4);
	return 0;
}
