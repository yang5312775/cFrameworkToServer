#include"mysql_connection_pool.h"

sqlConfig sqlC;
list * slq_list = NULL;
pthread_spinlock_t sql_lock;
int g_mysql_conn_enable = 0;

static int mysqlReconnect(MysqlBlock * block);
static int mysqlDetectConnection(MysqlBlock* block);

int mysqlConnectionPoolInit(char * mysqlIP , char * mysqlPort , char * mysqlAccount , char * mysqlPassword , char * databaseName , char * mysqlConnectionTimeout , char * connectCount)
{
	int i = 0;
	MysqlBlock * block = NULL;
	memset(&sqlC , 0 , sizeof(sqlConfig));
	strcpy(sqlC.ip , mysqlIP);
	strcpy(sqlC.account, mysqlAccount);
	strcpy(sqlC.password, mysqlPassword);
	strcpy(sqlC.databaseName, databaseName);
	sqlC.port = atoi(mysqlPort);
	sqlC.connectionTimeout = atoi(mysqlConnectionTimeout);
	sqlC.connectCount = atoi(connectCount);
	slq_list = listCreate();
	pthread_spin_init(&sql_lock, NULL);
	for (i = 0; i < sqlC.connectCount; i++)
	{
		block = (MysqlBlock *)malloc(sizeof(MysqlBlock));
		memset(block, 0, sizeof(MysqlBlock));
		mysql_init(&(block->sql));
		block->conn = mysql_real_connect(&(block->sql), sqlC.ip, sqlC.account, sqlC.password, sqlC.databaseName, sqlC.port, 0, 0);
		if (block->conn == NULL)
		{
			log_print(L_ERROR , "connection pool  init connection fail errcode:%s\n", mysql_error(&(block->sql)));
			mysqlConnectionPoolUnInit();
			return ERR_DATABASE_CONNECT_FAIL;
		}
		mysql_options(block->conn, MYSQL_INIT_COMMAND, "SET autocommit=0");
		block->lastUseTime = time(NULL);
		listAddNodeToTail(slq_list, block);
	}
	g_mysql_conn_enable = 1;
	return 0;
}

int mysqlConnectionPoolUnInit(void)
{
	MysqlBlock * block = NULL;
	if (g_mysql_conn_enable == 0)
		return 0;
	for (;;)
	{
		block = listPopNodeFromHead(slq_list);
		if (block == NULL)
			break;
		mysql_close(block->conn);
		free(block);
	}
	pthread_spin_lock(&sql_lock);
	pthread_spin_destroy(&sql_lock);
	g_mysql_conn_enable = 0;
	return 0;
}

MysqlBlock * mysqlConnectionPool_GetOneConn(void)
{
	MysqlBlock * block = NULL;
	if (g_mysql_conn_enable == 0)
		return NULL;
	for (;;)
	{
		pthread_spin_lock(&sql_lock);
		block = listPopNodeFromHead(slq_list);
		pthread_spin_unlock(&sql_lock);
		if (block != NULL)
		{
			if ((time(NULL) - block->lastUseTime) > sqlC.connectionTimeout)
			{
				if (mysqlReconnect(block))
				{
					log_print(L_WARNING, "mysql conn reconnect fail , free a connection !!!\n");
					free(block);
					continue;
				}
			}
			return block;
		}
		else 
		{
			log_print(L_WARNING, "all connention be used , new a connect now!!!!!!\n");
			block = (MysqlBlock *)malloc(sizeof(MysqlBlock));
			memset(block, 0, sizeof(MysqlBlock));
			if (mysqlReconnect(block))
			{
				log_print(L_WARNING, "new mysql conn reconnect fail , free  it!!!\n");
				free(block);
				continue;
			}
			return block;
		}
	}
}

void mysqlConnectionPool_FreeOneConn(MysqlBlock * block)
{
	if (g_mysql_conn_enable == 0)
		return;
	pthread_spin_lock(&sql_lock);
	block = listAddNodeToTail(slq_list , block);
	pthread_spin_unlock(&sql_lock);
}

static int mysqlDetectConnection(MysqlBlock* block)
{
	MYSQL * conn = block->conn;
	MYSQL_RES *result = NULL;
	int state = mysql_query(conn, "select 1;");//执行SQL语句  
	if (state != 0)
	{
		log_print(L_ERROR, "%s\n", mysql_error(conn));
		return -1;
	}
	result = mysql_store_result(conn);//得到查询结果  
	if (result == (MYSQL_RES *)NULL)
	{
		log_print(L_ERROR, "%s\n", mysql_error(conn));
		return -1;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
		result = NULL;
	}
	return 0;
}

static int mysqlReconnect(MysqlBlock * block)
{
	if(block != NULL)
		mysql_close(block->conn);
	memset(block, 0, sizeof(MysqlBlock));
	mysql_init(&(block->sql));
	block->conn = mysql_real_connect(&(block->sql), sqlC.ip, sqlC.account, sqlC.password, sqlC.databaseName, sqlC.port, 0, 0);
	if (block->conn == NULL)
		return -1;
	block->lastUseTime = time(NULL);
	return 0;
}
