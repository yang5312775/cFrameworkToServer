#include"socket_server.h"
#include"error.h"
aeEventLoop  * g_ael = NULL;
unsigned short serverPort = 0;


void aeSocketRead(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);

int getSocketSendBuff(unsigned char * dataInput, int inputLen, int maxLen, char requestType, char encryptType, char keepAliveFlag)
{
	sock_head s_h;
	enc_head  e_h;
	int lenR = 0;
	int encdLen = 0;
	int totalLength = 0;
	unsigned int timeS = 0;
	if (dataInput == NULL)
		return -5;
	if (encryptType == 0)
	{
		if (maxLen < (inputLen + sizeof(sock_head)))
			return -3;
	}
	else
	{
		if (maxLen < (inputLen + sizeof(sock_head) + sizeof(enc_head)))
			return -4;
	}
	timeS = time(0);
	timeS = ENDIAN_DWRD(timeS);
	memset(&s_h, 0, sizeof(sock_head));
	memset(&e_h, 0, sizeof(enc_head));
	s_h.magicWord[0] = 's'; s_h.magicWord[1] = 'o'; s_h.magicWord[2] = 's';
	s_h.requestType = requestType;
	s_h.keepAlive = keepAliveFlag;
	s_h.encFlag = encryptType;
	memcpy(s_h.time, &timeS, sizeof(s_h.time));

	if (s_h.encFlag == 0)
	{
		lenR = ENDIAN_DWRD(inputLen);
		memcpy(s_h.length, &lenR, sizeof(s_h.length));
		memcpy(dataInput + sizeof(sock_head), dataInput, inputLen);
		memcpy(dataInput, &s_h, sizeof(sock_head));
		return (sizeof(sock_head) + inputLen);
	}

	encdLen = encryptData(dataInput, inputLen, maxLen, encryptType);
	if (encdLen < 0)
		return encdLen;
	lenR = ENDIAN_DWRD(encdLen);
	memcpy(s_h.length, &lenR, sizeof(s_h.length));

	memcpy(dataInput + sizeof(sock_head), dataInput, encdLen);
	memcpy(dataInput, &s_h, sizeof(sock_head));
	return (sizeof(sock_head) + encdLen);
}

int parseRecvBuff(unsigned char * dataInput, int inputLen, sock_head * s_h)
{
	int length = 0;
	int retLen = 0;
	if (inputLen < sizeof(sock_head))
		return -6;
	memcpy(s_h, dataInput, sizeof(sock_head));

	length = *(int *)s_h->length;
	length = ENDIAN_DWRD(length);
	if (inputLen != (sizeof(sock_head) + length))
		return -6;

	if (s_h->encFlag == 0)
	{
		memcpy(dataInput, dataInput + sizeof(sock_head), length);
		return length;
	}

	retLen = decryptData(dataInput + sizeof(sock_head), ENC_TYPE_AES);
	if (retLen < 0)
		return retLen;
	memcpy(dataInput, dataInput + sizeof(sock_head), retLen);
	return retLen;
}

int isRecvOK(unsigned char * dataInput, int inputLen)
{
	int length = 0;
	sock_head * s_h = dataInput;
	if (s_h->magicWord[0] != 's' || s_h->magicWord[1] != 'o' || s_h->magicWord[2] != 's')
		return -1;
	if (inputLen < sizeof(sock_head))
		return -2;	
	length = *(int *)s_h->length;
	length = ENDIAN_DWRD(length);
	if (inputLen != (sizeof(sock_head) + length))
		return -2;
	return 0;
}




void json_handle(void * data)
{
	fd_state * fd_s = data;
	char *errString = NULL;

	
	cJSON *json = NULL;
	json = cJSON_Parse(fd_s->buffer);
	if (json == NULL)
	{
		fd_s->errcode = ERR_JSON_PARSE_FAIL;
		goto end;
	}

	cJSON *temp = cJSON_GetObjectItem(json, "route");
	char * route = temp->valuestring;
	//	printf("route  : %s\n" , temp->valuestring);
	temp = cJSON_GetObjectItem(json, "prm");
	char * prm = cJSON_Print(temp);
	memset(fd_s->buffer , 0 , MAX_LINE);
	memcpy(fd_s->buffer , prm , strlen(prm));
	fd_s->buffer_used = strlen(prm);
	free(prm);
	fd_s->fd_sql = mysqlConnectionPool_GetOneConn();		//获取一个数据库连接
	route_fun * fun = get_function_route(route);
	if (fun == NULL)
	{
		fd_s->errcode = ERR_UNDEFINED_REQUEST_ROUTE;
		goto end;
	}

	(fun)(fd_s);

end:

	if (fd_s->errcode != 0)
	{
		errString = makeErrorJson(fd_s->errcode , gerErrMsg(fd_s->errcode));
		memcpy(fd_s->buffer, errString, strlen(errString));
		free(errString);
		fd_s->buffer_used = strlen(errString);
	}
	fd_s->buffer_used = getSocketSendBuff(fd_s->buffer , fd_s->buffer_used , MAX_LINE, fd_s->sh.requestType , fd_s->sh.encFlag , fd_s->sh.keepAlive);
	fd_s->n_written = fd_s->buffer_used;
	if(json != NULL)
		cJSON_Delete(json);
	if (fd_s->fd_sql != NULL)
	{
//		mysql_commit(fd_s->fd_sql);   //手动提交数据库   这里有问题
		mysqlConnectionPool_FreeOneConn(fd_s->fd_sql);//释放一个数据库连接
	}
	return;
}
//stream 结构体暂定很简单如下：
//2byte route_id  
//nbyte data
void stream_handle(void * data)
{
	fd_state * fd_s = data;
	char *errString = NULL;
	char idChar[10];
	memset(idChar , 0 , 6);
	strcat(idChar , "r_");
	unsigned short id = *(unsigned short *)fd_s->buffer;
	id = ENDIAN_WORD(id);
	itoa(id, idChar + 2, 10);
	fd_s->buffer_used -= 2;
	memcpy(fd_s->buffer , fd_s->buffer +2 , fd_s->buffer_used);
	fd_s->fd_sql = mysqlConnectionPool_GetOneConn();		//获取一个数据库连接
	route_fun * fun = get_function_route(idChar);
	if (fun == NULL)
	{
		fd_s->errcode = ERR_UNDEFINED_REQUEST_ROUTE;
		goto end;
	}

	(fun)(fd_s);

end:
	if (fd_s->errcode != 0)
	{
		errString = gerErrMsg(fd_s->errcode);
		memcpy(fd_s->buffer, errString, strlen(errString));
		fd_s->buffer_used  = strlen(errString);
	}
	fd_s->buffer_used = getSocketSendBuff(fd_s->buffer, fd_s->buffer_used, MAX_LINE, fd_s->sh.requestType, fd_s->sh.encFlag, fd_s->sh.keepAlive);
	fd_s->n_written = fd_s->buffer_used;
	mysql_commit(fd_s->fd_sql);   //手动提交数据库
	mysqlConnectionPool_FreeOneConn(fd_s->fd_sql);//释放一个数据库连接
	return;
}

void undefined_handle(void * data)
{
	fd_state * fd_s = data;
	fd_s->errcode = ERR_UNDEFINED_REQUEST_TYPE;
	fd_s->buffer[0] = fd_s->errcode;
	fd_s->buffer_used = fd_s->n_written = 1;
	fd_s->sh.keepAlive = 0;
}

route_fun * get_handle_function(unsigned char type)
{
	switch (type)
	{
	case REQUEST_TYPE_SAREAM:
		return stream_handle;
		break;
	case REQUEST_TYPE_JSON:
		return json_handle;
		break;
	default:
		return undefined_handle;
	};
}

void socketWrite(void * data)
{
	int ret = -1 , fdTemp;
	fd_state * fd_s = data;
	int result;
	for (;;)
	{
		result = send(fd_s->fd, fd_s->buffer + fd_s->write_upto, fd_s->n_written - fd_s->write_upto, 0);
		if (result <= 0)
		{
			aeDeleteFileEvent(g_ael, fd_s->fd, AE_READABLE);
			break;
		}
			
		fd_s->write_upto += result;
		if (fd_s->write_upto == fd_s->n_written)
		{
			if (fd_s->sh.keepAlive == 0)
				goto end;
			else
			{
				socket_set_nonblock(fd_s->fd);
				memset(fd_s->buffer, 0, MAX_LINE);
				fd_s->buffer_used = fd_s->n_written = fd_s->write_upto = 0;
				fd_s->readFlag = 0;
				return;
			}
		}
		else
			printf("data no send completely  one time!!!\n");
	}

	printf("send data fail!!![%d]\n" , result);
end:
	socket_close(fd_s->fd);
	FREE(fd_s);

}

void handle_received_data(void * data)
{
	int retLen = 0;
	char * errcode = NULL;
	fd_state * fd_s = data;
	retLen = parseRecvBuff(fd_s->buffer, fd_s->buffer_used, &fd_s->sh);
	if (retLen < 0)
	{
		printf("parseRecvBuff fail!!!\n");
		fd_s->errcode = retLen;
		fd_s->buffer[0] = fd_s->errcode;
		fd_s->buffer_used = fd_s->n_written = 1;
		goto end;
	}
	fd_s->buffer_used = retLen;
	get_handle_function(fd_s->sh.requestType)(fd_s);
end:
	socketWrite(fd_s);
}

void aeSocketRead(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
	int ret = -1;
	int result;
	sock_head * s_h = NULL;
	fd_state * fd_s = clientData;
	if (fd_s->readFlag != 0)
		return;
	for (;;)
	{
		result = recv(fd, fd_s->buffer + fd_s->buffer_used, sizeof(fd_s->buffer) - fd_s->buffer_used, 0);
		if (result <= 0)
		{
			printf("result <= 0");
			break;
		}
		fd_s->buffer_used += result;
		ret = isRecvOK(fd_s->buffer, fd_s->buffer_used);
//		printf("%d|" ,ret);
		if (ret == -1)
		{
			printf("unrecognized receive data\n");
			result = -1;
			goto err;
		}
		if (ret == -2)
		{
			printf("data not receive complete\n");
			continue;
		}	
		s_h = fd_s->buffer;
		if(s_h->keepAlive == 0)
			aeDeleteFileEvent(g_ael, fd_s->fd, AE_READABLE);
		fd_s->readFlag = 1;
		socket_set_block(fd);
		socket_set_sendtimeo(fd, 2000);
		threadpool_add(handle_received_data, fd_s, 0);
		return;
	}
	if (result < 0) 
	{
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
			;
	}
err:
	printf("recv fail  errcode[%d]\n", result);
	aeDeleteFileEvent(g_ael, fd, AE_READABLE);
	FREE(fd_s);
	socket_close(fd);
}

void aeDoAccept(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask)
{
	sockaddr_t addr;
	fd_state * fd_s = NULL;
	int newfd = socket_accept(fd, &addr);
	if (newfd < 0)
	{
		printf("accept fail!!!error  code is %d\n", newfd);
		return;
	}
	fd_s = MALLOC(sizeof(fd_state));
	memset(fd_s, 0, sizeof(fd_state));
	fd_s->fd = newfd;
	if (AE_ERR == aeCreateFileEvent(g_ael, newfd, AE_READABLE, aeSocketRead, fd_s))
	{
		printf("aeCreateFileEvent AE_READABLE FAIL  FD===%d\n" , newfd);
		socket_close(newfd);
		FREE(fd_s);
		return;
	}
	socket_set_nonblock(newfd);
}

int DefaultTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData) {
//	sendNoBlockMsg();
	return 1000;  //返回值是下一次定时设置时间，毫秒
}

int socket_server_start(char * ip , short port , int backlog) 
{
	g_ael = aeCreateEventLoop(3000);
	if (g_ael == NULL)
		return ERR_CREATE_EVENTLOOP_FAIL;
	serverPort = port;
	socket_t server_listener = socket_listen(ip, port, backlog);
	socket_set_nonblock(server_listener);
	aeCreateFileEvent(g_ael, server_listener, AE_READABLE, aeDoAccept, NULL);
//	aeCreateTimeEvent(g_ael , 1000 , DefaultTimeProc, NULL , NULL);//增加一个定时器任务，会迫使select epoll等模型设置超时时间，超时时间 小于等于 定时器定时时间

	aeMain(g_ael);

	aeDeleteEventLoop(g_ael);


}