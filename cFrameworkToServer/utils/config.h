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
	int authPushPoolCount;          //授权池数量
	int authPushPoolCapacity[10];   //每个授权池的容量
	int authPushPoolRateX[10];		//授权池的速率倍数
	int authPushPoolBandwidth;		//授权推送带宽
	char multicastIP[16];
	char multicastPORT[6];
}Config;

extern Config serverConfig;
//装载config文件
int loadConfig(char * configFilePath);


//new version config
typedef struct node  conf_node;

struct node {
	char * left;
	char * right;
	conf_node * next;
};

#endif