#ifndef _SOCKET_SERVER_H__
#define _SOCKET_SERVER_H__
#include<basic.h>
#include"mysql_connection_pool.h"

#define MAX_LINE 1024

#define REQUEST_TYPE_SAREAM	0
#define REQUEST_TYPE_JSON	1


typedef struct {
	unsigned char	magicWord[3];   //���ݱ�ʶ
	unsigned char	requestType;   //socket �������� Ŀǰ��  json���� stream����
	unsigned char	keepAlive;     //�Ƿ����ó�����
	unsigned char	encFlag;       //�Ƿ���ܴ���
	unsigned char	time[4];
	unsigned char	length[4];	   //����ͷ����������ݳ���
}sock_head;
//socket���͵����ݷ����������һ���Ǽ������ݣ�һ���ǷǼ������ݣ���� encFlag ==1 �����͵�����Ϊ  1.sock_head 2.enc_head 3.��������
//���encFlag ==0 ������1.sock_head   2.��������

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