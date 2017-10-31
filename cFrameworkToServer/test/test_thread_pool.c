#include<basic.h>

#define THREAD 10
//
// 测试 thread_pool.h 接口的使用
//
int tasks = 0, done = 0;
pthread_spinlock_t lock;

void dummy_task(void *arg) {
	Sleep(500);
	pthread_spin_lock(&lock);
	printf("work!!\n");
	done++;
	pthread_spin_unlock(&lock);
}

void test_threadpool(void) {

	threadpool_t *pool;

//	pthread_mutex_init(&lock, NULL);

//	threadpool_create(THREAD, 0);
//	fprintf(stderr, "Pool started with %d threads \n", THREAD);
//	Sleep(5000);
	int i = 0;
	for (;;)
	{
		int ret = threadpool_add(&dummy_task, NULL, 0);
		printf("add task\n");
		if(i++ == 100)
			break;
	}
	Sleep(1000);
	printf("thread shutdown start\n");
//	int ret = threadpool_destroy(1);
	printf("thread shutdown end\n");
}

#if 0
//test
#define THREAD 32
#define QUEUE  256

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "threadpool.h"

int tasks = 0, done = 0;
pthread_mutex_t lock;

void dummy_task(void *arg) {
	usleep(10000);
	pthread_mutex_lock(&lock);
	done++;
	pthread_mutex_unlock(&lock);
}

int main(int argc, char **argv)
{
	threadpool_t *pool;

	pthread_mutex_init(&lock, NULL);

	assert((pool = threadpool_create(THREAD, QUEUE, 0)) != NULL);
	fprintf(stderr, "Pool started with %d threads and "
		"queue size of %d\n", THREAD, QUEUE);

	while (threadpool_add(pool, &dummy_task, NULL, 0) == 0) {
		pthread_mutex_lock(&lock);
		tasks++;
		pthread_mutex_unlock(&lock);
	}

	fprintf(stderr, "Added %d tasks\n", tasks);

	while ((tasks / 2) > done) {
		usleep(10000);
	}
	assert(threadpool_destroy(pool, 0) == 0);
	fprintf(stderr, "Did %d tasks\n", done);

	return 0;
}

//shutdown
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "threadpool.h"

#define THREAD 4
#define SIZE   8192

threadpool_t *pool;
int left;
pthread_mutex_t lock;

int error;

void dummy_task(void *arg) {
	usleep(100);
	pthread_mutex_lock(&lock);
	left--;
	pthread_mutex_unlock(&lock);
}

int main(int argc, char **argv)
{
	int i;

	pthread_mutex_init(&lock, NULL);

	/* Testing immediate shutdown */
	left = SIZE;
	pool = threadpool_create(THREAD, SIZE, 0);
	for (i = 0; i < SIZE; i++) {
		assert(threadpool_add(pool, &dummy_task, NULL, 0) == 0);
	}
	assert(threadpool_destroy(pool, 0) == 0);
	assert(left > 0);

	/* Testing graceful shutdown */
	left = SIZE;
	pool = threadpool_create(THREAD, SIZE, 0);
	for (i = 0; i < SIZE; i++) {
		assert(threadpool_add(pool, &dummy_task, NULL, 0) == 0);
	}
	assert(threadpool_destroy(pool, threadpool_graceful) == 0);
	assert(left == 0);

	pthread_mutex_destroy(&lock);

	return 0;
}

//heavy

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "threadpool.h"

#define THREAD 4
#define SIZE   8192
#define QUEUES 64

/*
* Warning do not increase THREAD and QUEUES too much on 32-bit
* platforms: because of each thread (and there will be THREAD *
* QUEUES of them) will allocate its own stack (8 MB is the default on
* Linux), you'll quickly run out of virtual space.
*/

threadpool_t *pool[QUEUES];
int tasks[SIZE], left;
pthread_mutex_t lock;

int error;

void dummy_task(void *arg) {
	int *pi = (int *)arg;
	*pi += 1;

	if (*pi < QUEUES) {
		assert(threadpool_add(pool[*pi], &dummy_task, arg, 0) == 0);
	}
	else {
		pthread_mutex_lock(&lock);
		left--;
		pthread_mutex_unlock(&lock);
	}
}

int main(int argc, char **argv)
{
	int i, copy = 1;

	left = SIZE;
	pthread_mutex_init(&lock, NULL);

	for (i = 0; i < QUEUES; i++) {
		pool[i] = threadpool_create(THREAD, SIZE, 0);
		assert(pool[i] != NULL);
	}

	usleep(10);

	for (i = 0; i < SIZE; i++) {
		tasks[i] = 0;
		assert(threadpool_add(pool[0], &dummy_task, &(tasks[i]), 0) == 0);
	}

	while (copy > 0) {
		usleep(10);
		pthread_mutex_lock(&lock);
		copy = left;
		pthread_mutex_unlock(&lock);
	}

	for (i = 0; i < QUEUES; i++) {
		assert(threadpool_destroy(pool[i], 0) == 0);
	}

	pthread_mutex_destroy(&lock);

	return 0;
}




#endif