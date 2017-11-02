#include"socket_server.h"

aeEventLoop  * g_ael = NULL;
dict * socketserverDict = NULL;

void stream_handle(void * data)
{
	real_data_node  * s_d_node = data;
	printf("stream_handle\n");
	memset(s_d_node->fd_s->buffer, 0, sizeof(s_d_node->fd_s->buffer));
	strcpy(s_d_node->fd_s->buffer, "not support yet");
	s_d_node->fd_s->n_written = strlen("not support yet");//test
	free(s_d_node->data);
	free(s_d_node);
}

void json_handle(void * data)
{
	char errString[128];
	memset(errString, 0, 128);
	real_data_node  * s_d_node = data;
	//	printf("json_handle\n");
	cJSON *json = NULL;
	json = cJSON_Parse(s_d_node->data);
	if (json == NULL)
	{
		strcpy(errString, "bad request");
		goto err;
	}

	cJSON *temp = cJSON_GetObjectItem(json, "route");
	char * route = temp->valuestring;
	//	printf("route  : %s\n" , temp->valuestring);
	temp = cJSON_GetObjectItem(json, "prm");
	char * prm = cJSON_Print(temp);
	//	printf("prm :\n %s\n" , prm);
	free(s_d_node->data);
	s_d_node->data = prm;
	s_d_node->len = strlen(prm);
	route_fun * fun = get_function_route(route);
	if (fun == NULL)
	{
		strcpy(errString, "unregist route");
		goto err;
	}

	(get_function_route(route))(s_d_node);

	memset(s_d_node->fd_s->buffer, 0, sizeof(s_d_node->fd_s->buffer));
	memcpy(s_d_node->fd_s->buffer, s_d_node->data, s_d_node->len);
	s_d_node->fd_s->n_written = s_d_node->len;
	cJSON_Delete(json);
	free(s_d_node->data);
	free(s_d_node);
	return;
err:
	memset(s_d_node->fd_s->buffer, 0, sizeof(s_d_node->fd_s->buffer));
	strcpy(s_d_node->fd_s->buffer, errString);
	s_d_node->fd_s->n_written = strlen(errString);//test
	s_d_node->fd_s->kepp_alive = 0;
	cJSON_Delete(json);
	free(s_d_node->data);
	free(s_d_node);
}
void undefined_handle(void * data)
{
	real_data_node  * s_d_node = data;
	//	printf("undefined_handle\n");
	memset(s_d_node->fd_s->buffer, 0, sizeof(s_d_node->fd_s->buffer));
	strcpy(s_d_node->fd_s->buffer, "undefined request");
	s_d_node->fd_s->n_written = strlen("undefined request");//test
	s_d_node->fd_s->kepp_alive = 0;
	free(s_d_node->data);
	free(s_d_node);
}

route_fun * get_handle_function(unsigned char type)
{
	switch (type)
	{
	case 0:
		return stream_handle;
		break;
	case 1:
		return json_handle;
		break;
	default:
		return undefined_handle;
	};
}

void handle_socket_receive_data(void * data)
{
	fd_state * fd_s = data;
	socket_head * s_h = fd_s->buffer;
	real_data_node  * s_d_node = malloc(sizeof(real_data_node));
	//	printf("handle_socket_receive_data start\n");
	//	printf("data type===[%d]\n", s_h->type);
	//	printf("data enc_type===[%d]\n", s_h->enc_type);
	//	printf("data keepalive===[%d]\n", s_h->keep_alive);
	//	printf("data length===[%d]\n", s_h->length);
	//	printf("handle_socket_receive_data end\n");
	if (s_h->enc_type)//decrypt , not think yet
		;

	s_d_node->len = s_h->length - sizeof(socket_head);
	s_d_node->data = malloc(s_d_node->len);
	memcpy(s_d_node->data, fd_s->buffer + sizeof(socket_head), s_d_node->len);
	s_d_node->fd_s = fd_s;
	get_handle_function(s_h->type)(s_d_node);
}

void aeSocketWrite(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
	int ret = -1;
	char fdChar[16];
	socket_head * s_h = NULL;

	memset(fdChar, 0, 16);
	itoa(fd, fdChar, 10);
//	printf("file write fd======%s\n", fdChar);
	fd_state * fd_s = dictFetchValue(socketserverDict, fdChar);
	if (fd_s == NULL)
	{
		printf("no this fd state node in socketserverDict dict \n");
		return;
	}
	if (fd_s->n_written == 0)
	{
//		printf("no data need write!!!\n");
		return;
	}
	int result;
	for (;;)
	{
		result = send(fd, fd_s->buffer + fd_s->write_upto, fd_s->n_written - fd_s->write_upto, 0);
//		printf("send   bytes[%d]\n" , result);
		if (result <= 0)
			break;
		fd_s->write_upto += result;
		if (fd_s->write_upto == fd_s->n_written)
		{
//			printf("send buff finish!!\n");
			aeDeleteFileEvent(g_ael, fd, AE_WRITABLE);		
			if (fd_s->kepp_alive == 0)
			{
				dictDelete(socketserverDict, fdChar);
				aeDeleteFileEvent(g_ael, fd, AE_READABLE);
				socket_close(fd);
			}
			else
			{
				memset(fd_s, 0, sizeof(*fd_s));
				fd_s->kepp_alive = 1;
			}
				
			return;
		}
		else
			printf("data no send completely  one time!!!\n");
	}
	if (result <= 0)
	{
		printf("send data fail!!![%d]\n" , result);
		dictDelete(socketserverDict, fdChar);
		aeDeleteFileEvent(g_ael, fd, AE_WRITABLE);
		aeDeleteFileEvent(g_ael, fd, AE_READABLE);
		socket_close(fd);
	}
}

void aeSocketRead(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
	int ret = -1;
	char fdChar[16];
	socket_head * s_h =NULL;

	memset(fdChar , 0 , 16);
	itoa(fd, fdChar, 10);
//	printf("file read fd======%s\n" , fdChar);
	fd_state * fd_s = dictFetchValue(socketserverDict , fdChar);
	int result;
	for (;;)
	{
		result = recv(fd, fd_s->buffer + fd_s->buffer_used , sizeof(fd_s->buffer) - fd_s->buffer_used , 0);
//		printf("recv   bytes[%d]\n", result);
		if (result <= 0)
			break;
		fd_s->buffer_used += result;
		s_h = fd_s->buffer;
		if (s_h->length == fd_s->buffer_used)
		{
			fd_s->kepp_alive = s_h->keep_alive;
//			printf("data receive finish!!\n");
			aeCreateFileEvent(g_ael, fd_s->fd, AE_WRITABLE, aeSocketWrite, NULL);
			threadpool_add(handle_socket_receive_data , fd_s , 0);
			break;
		}
			
	}
	if (result == 0) {
		printf("recv fail  errcode[%d]\n", result);
		dictDelete(socketserverDict , fdChar);
		aeDeleteFileEvent(g_ael, fd, AE_READABLE);
		aeDeleteFileEvent(g_ael, fd, AE_WRITABLE);
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
			printf("recv fail  errcode[%d]\n" , result);
			dictDelete(socketserverDict, fdChar);
			aeDeleteFileEvent(g_ael, fd, AE_READABLE);
			aeDeleteFileEvent(g_ael, fd, AE_WRITABLE);
			socket_close(fd);
		}
	}
}

void aeDoAccept(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
	sockaddr_t addr;
//	printf("new connect!!!! ,  need accept!!!!\n");
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
//	printf("new fd ====[%s]\n", newfd);
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