#include"thread_pool.h"
// async_run - 开启一个自销毁的线程 运行 run
// run		: 运行的主体
// arg		: run的参数
// return	: >= RETURN_OK 表示成功
//
inline int async_run_(node_f run, void * arg)
{
	pthread_t tid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (pthread_create(&tid, &attr, (start_f)run, arg) < 0) {
		pthread_attr_destroy(&attr);
		return ERR_THREAD_CREATE_FAIL;
	}

	pthread_attr_destroy(&attr);
	return RETURN_OK;
}