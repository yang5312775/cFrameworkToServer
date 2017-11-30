#ifndef mysqlConnectPool_h
#define mysqlConnectPool_h
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<mysql.h>
#include"log.h"
#include"config.h"
#include"basic_struct\list.h"

#define MYSQL_CONNECTION_POLL_IS_FULL  100



typedef struct mysqlblock {
	MYSQL sql;
	MYSQL* conn;
	unsigned int lastUseTime;
}MysqlBlock;

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

MysqlBlock * mysqlConnectionPool_GetOneConn(void);

void mysqlConnectionPool_FreeOneConn(MysqlBlock * conn);


#endif