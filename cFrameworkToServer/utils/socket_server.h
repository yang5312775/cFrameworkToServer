#ifndef _SOCKET_SERVER_H_
#define _SOCKET_SERVER_H_
#include<basic.h>

typedef struct {
	unsigned char	type;
	unsigned char   enc_type;
	unsigned char	keep_alive;
	unsigned char	length;
}socket_head;

int socket_server_start(char * ip, short port, int backlog);

#endif