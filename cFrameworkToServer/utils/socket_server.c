#include"socket_server.h"

aeEventLoop  * g_ael = NULL;
dict * socketserverDict = NULL;
#define MAX_LINE 16000

typedef struct{
	int fd;
	char buffer[MAX_LINE];
	size_t buffer_used;
	size_t n_written;
	size_t write_upto;
	char	kepp_alive;
}fd_state;




void aeSocketWrite(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
	int ret = -1;
	char fdChar[16];
	socket_head * s_h = NULL;

	memset(fdChar, 0, 16);
	itoa(fd, fdChar, 10);
	printf("file write fd======%s\n", fdChar);
	fd_state * fd_s = dictFetchValue(socketserverDict, fdChar);
	if (fd_s->n_written == 0)
	{
		printf("no data need write!!!\n");
		return;
	}
	int result;
	for (;;)
	{
		result = send(fd, fd_s->buffer + fd_s->write_upto, sizeof(fd_s->buffer), 0);
		if (result <= 0)
			break;
		fd_s->write_upto += result;
		if (fd_s->write_upto == fd_s->n_written)
		{
			printf("send buff finish!!\n");
			aeDeleteFileEvent(g_ael, fd, AE_WRITABLE);
			if (fd_s->kepp_alive == 0)
			{
				aeDeleteFileEvent(g_ael, fd, AE_READABLE);
				socket_close(fd);
			}
			return;
		}
		else
			printf("data no send completely  one time!!!\n");
	}
	if (result <= 0)
	{
		printf("send data fail!!!\n");
		aeDeleteFileEvent(g_ael, fd, AE_WRITABLE);
		aeDeleteFileEvent(g_ael, fd, AE_READABLE);
		socket_close(fd);
	}
}

void handle_socket_receive_data(void * data)
{
	fd_state * fd_s = data;
	socket_head * s_h = fd_s->buffer;

	printf("handle_socket_receive_data start\n");
	printf("data type===[%d]\n", s_h->type);
	printf("data enc_type===[%d]\n", s_h->enc_type);
	printf("data keepalive===[%d]\n", s_h->keep_alive);
	printf("data length===[%d]\n", s_h->length);
	printf("data is :\n");
	for (int i = 0; i < s_h->length; i++)
		printf("[%02x] ", fd_s->buffer[i]);
	printf("\ndata end\n");
	Sleep(100);
	printf("handle_socket_receive_data end\n");
	fd_s->n_written = s_h->length;
	
}


void aeSocketRead(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
	int ret = -1;
	char fdChar[16];
	socket_head * s_h =NULL;

	memset(fdChar , 0 , 16);
	itoa(fd, fdChar, 10);
	printf("file read fd======%s\n" , fdChar);
	fd_state * fd_s = dictFetchValue(socketserverDict , fdChar);
	int result;
	for (;;)
	{
		result = recv(fd, fd_s->buffer + fd_s->buffer_used , sizeof(fd_s->buffer), 0);
		if (result <= 0)
			break;
		fd_s->buffer_used += result;
		s_h = fd_s->buffer;

		if (s_h->length == fd_s->buffer_used)
		{
			fd_s->kepp_alive = s_h->keep_alive;
			printf("data receive finish!!\n");
			aeCreateFileEvent(g_ael, fd_s->fd, AE_WRITABLE, aeSocketWrite, NULL);
			threadpool_add(handle_socket_receive_data , fd_s , 0);
			break;
		}
			
	}
	if (result == 0) {
		dictDelete(socketserverDict , fdChar);
		aeDeleteFileEvent(g_ael, fd, AE_READABLE);
		socket_close(fd);
	}
	else if (result < 0) {
		if (errno == EAGAIN) // XXXX use evutil macro
		{
			printf("data receive not complete!!\n");
			return;
		}
		else if (errno == EINTR)
		{
			printf("interrupt!!\n");
			return;
		}
		else
		{
			dictDelete(socketserverDict, fdChar);
			aeDeleteFileEvent(g_ael, fd, AE_READABLE);
			socket_close(fd);
		}
	}
}

void aeDoAccept(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
	sockaddr_t addr;
	printf("new connect!!!! ,  need accept!!!!\n");
	int fdInt = socket_accept(fd, &addr);
	if (fdInt < 0)
	{
		printf("accept fail!!!error  code is %d\n" , fdInt);
		return;
	}
	char * newfd = malloc(16);
	memset(newfd, 0, 16);
	itoa(fdInt , newfd , 10);
	socket_set_nonblock(*newfd);
	printf("new fd ====[%s]\n", newfd);
	fd_state * fd_s = malloc(sizeof(fd_state));
	memset(fd_s , 0 , sizeof(fd_state));
	fd_s->fd = fdInt;
	dictAdd(socketserverDict , newfd , fd_s);
	aeCreateFileEvent(g_ael, fdInt, AE_READABLE, aeSocketRead, NULL);
}



//int tiemProc(struct aeEventLoop *eventLoop, long long id, void *clientData) {
//	printf("call tiemProc  in [%d] time stamp\n", time(0));
//}

int socket_server_start(char * ip , short port , int backlog) 
{
	g_ael = aeCreateEventLoop(1024);
	if (g_ael == NULL)
		return ERR_CREATE_EVENTLOOP_FAIL;
	socketserverDict = dictCreateEx();

	int server_listener = socket_listen(ip, port, backlog);
	socket_set_nonblock(server_listener);
	aeCreateFileEvent(g_ael, server_listener, AE_READABLE, aeDoAccept, NULL);
	aeMain(g_ael);
	aeDeleteEventLoop(g_ael);

}