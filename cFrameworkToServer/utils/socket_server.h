#ifndef _SOCKET_SERVER_H__
#define _SOCKET_SERVER_H__
#include<basic.h>
#include"mysql_connection_pool.h"

#define MAX_LINE 1024

#define REQUEST_TYPE_SAREAM	0
#define REQUEST_TYPE_JSON	1


typedef struct {
	unsigned char	magicWord[3];   //数据标识
	unsigned char	requestType;   //socket 请求类型 目前分  json请求 stream请求
	unsigned char	keepAlive;     //是否启用长连接
	unsigned char	encFlag;       //是否加密传送
	unsigned char	time[4];
	unsigned char	length[4];	   //跟在头部后面的数据长度
}sock_head;
//socket传送的数据分两种情况，一种是加密数据，一种是非加密数据，如果 encFlag ==1 ，则传送的数据为  1.sock_head 2.enc_head 3.加密数据
//如果encFlag ==0 ，则传送1.sock_head   2.明文数据

typedef struct {
	int fd;
	char buffer[MAX_LINE];
	char readFlag;
	size_t buffer_used;
	size_t n_written;
	size_t write_upto;
	sock_head sh;
	int errcode;
	MysqlBlock* fd_sql;
}fd_state;

int socket_server_start(char * ip, short port, int backlog);



#endif