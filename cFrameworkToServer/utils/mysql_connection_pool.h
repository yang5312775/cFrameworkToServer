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

int mysqlConnectionPool_init(Config * conf);
int mysqlConnectionPool_close(void);
MYSQL * mysqlConnectionPool_GetOneConn(void);
int mysqlConnectionPool_FreeOneConn(MYSQL * conn);
int testConnection(MYSQL* conn);
#endif