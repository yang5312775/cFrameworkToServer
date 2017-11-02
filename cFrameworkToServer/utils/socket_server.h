#ifndef _SOCKET_SERVER_H_
#define _SOCKET_SERVER_H_
#include<basic.h>

#define MAX_LINE 4096

typedef struct {
	unsigned char	type;
	unsigned char   enc_type;
	unsigned char	keep_alive;
	unsigned char	length;
}socket_head;

typedef struct {
	int fd;
	char buffer[MAX_LINE];
	size_t buffer_used;
	size_t n_written;
	size_t write_upto;
	char	kepp_alive;
}fd_state;

typedef struct {
	char * data;
	int len;
	fd_state * fd_s;
}real_data_node;



int socket_server_start(char * ip, short port, int backlog);

#endif