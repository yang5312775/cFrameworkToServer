#include<event\ae.h>
#include<socket_basic.h>
aeEventLoop  * ael = NULL;
socket_t listener = 0;
//
// 测试 ae.h 接口的使用
//
char buffer[10000];
#define UNUSED(V) ((void) V)
void aeFileRead(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);


void aeFileWrite(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
//	printf("call aeFileWrite  in [%d] time stamp\n", time(0));
	send(fd , buffer , 54 , 0);
	aeDeleteFileEvent(ael, fd, AE_READABLE);
	aeDeleteFileEvent(ael , fd , AE_WRITABLE);
	socket_close(fd);
//	aeCreateFileEvent(ael, fd, AE_READABLE, aeFileRead, NULL);
}


void aeFileRead(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
	sockaddr_t addr;
	int ret = -1;
//	printf("call aeFileRead [%d] in [%d] time stamp\n",fd ,  time(0));
	if (fd == listener)
	{
	//	printf("new connect!!!! ,  need accept!!!!\n");
		int newfd = socket_accept(fd , &addr);
	//	printf("new fd ====[%d]\n" , newfd);
		aeCreateFileEvent(ael, newfd, AE_READABLE, aeFileRead, NULL);
		return;
	}
//	printf("fd[%d] have data to recevie!!!\n");
	memset(buffer , 0 , 10000);
	ret = recv(fd, buffer , 10000 , 0);
	if (ret == -1)
		aeDeleteFileEvent(ael , fd, AE_READABLE);
//	printf("recv data size[%d]\n" , ret);
	aeCreateFileEvent(ael, fd, AE_WRITABLE, aeFileWrite, NULL);

}


int tiemProc2(struct aeEventLoop *eventLoop, long long id, void *clientData) {
	int j;
	UNUSED(eventLoop);
	UNUSED(id);
	UNUSED(clientData);
	printf("call tiemProc  in [%d] time stamp\n", time(0));
}

void beforeSleep(struct aeEventLoop *eventLoop) {
	UNUSED(eventLoop);
	printf("call beforeSleep  in [%d] time stamp\n", time(0));
}

void afterSleep(struct aeEventLoop *eventLoop) {
	UNUSED(eventLoop);
	printf("call afterSleep  in [%d] time stamp\n", time(0));
}
void test_pool_event(void) {
	
	printf("sadfasdfasldfasdjlf!!!\n");
	ael = aeCreateEventLoop(1024);

//	aeCreateTimeEvent(ael, 1, tiemProc, NULL, NULL);
	listener = socket_listen(NULL , 8888 , 20);
	socket_set_nonblock(listener);
	aeCreateFileEvent(ael , listener , AE_READABLE, aeFileRead , NULL);


//	aeSetBeforeSleepProc(ael, beforeSleep);
//	aeSetAfterSleepProc(ael, afterSleep);
	aeMain(ael);
	aeDeleteEventLoop(ael);

}