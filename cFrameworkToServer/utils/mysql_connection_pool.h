#ifndef mysqlConnectPool_h
#define mysqlConnectPool_h
#include<basic.h>
#include"log.h"
#include"config.h"
#define MYSQL_CONNECTION_POLL_IS_FULL  100


typedef struct {
	MYSQL sql;
	MYSQL* conn;
	unsigned char useFlag;
	unsigned int lastUseTime;
} MysqlBlock;

typedef struct {
	char ip[16];
	short port;
	char account[32];
	char password[32];
	char databaseName[32];
	int connectionTimeout;
	int connectCount;
}sqlConfig;

int mysqlConnectionPoolInit(char * mysqlIP, char * mysqlPort, char * mysqlAccount, char * mysqlPassword, char * databaseName, char * mysqlConnectionTimeout, char * connectCount);

int mysqlConnectionPoolUnInit(void);

MYSQL * mysqlConnectionPool_GetOneConn(void);

int mysqlConnectionPool_FreeOneConn(MYSQL * conn);

int testConnection(MYSQL* conn);
#endif