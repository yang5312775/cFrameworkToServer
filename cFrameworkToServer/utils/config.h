#ifndef config_h
#define config_h
#include<basic.h>

typedef struct {
	char logPath[256];
	char serverPort[6];
	char mysqlIP[16];
	char mysqlPort[6];
	char mysqlAccount[32];
	char mysqlPassword[32];
	int mysqlConnectionTimeout;
	char databaseName[32];
	int authPushPoolCount;          //��Ȩ������
	int authPushPoolCapacity[10];   //ÿ����Ȩ�ص�����
	int authPushPoolRateX[10];		//��Ȩ�ص����ʱ���
	int authPushPoolBandwidth;		//��Ȩ���ʹ���
	char multicastIP[16];
	char multicastPORT[6];
}Config;

extern Config serverConfig;
//װ��config�ļ�
int loadConfig(char * configFilePath);


//new version config
typedef struct node  conf_node;

struct node {
	char * left;
	char * right;
	conf_node * next;
};

#endif