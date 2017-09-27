#include"mysql_connection_pool.h"

#define MAX_MYSQL_CONNECTION 100




MysqlBlock mysqlBlock[MAX_MYSQL_CONNECTION];
pthread_mutex_t mutexMysqlPool;
void* check_connection(void* Param);

int testConnection(MYSQL* conn)
{
	MYSQL_RES *result = NULL;
	int state = mysql_query(conn, "select 1;");//执行SQL语句  
	if (state != 0)
	{
		DEBUG("%s\n", mysql_error(conn));
		return -1;
	}
	result = mysql_store_result(conn);//得到查询结果  
	if (result == (MYSQL_RES *)NULL)
	{
		DEBUG("%s\n", mysql_error(conn));
		return -1;
	}
	if (result != NULL)
	{
		mysql_free_result(result);
		result = NULL;
	}
	return 0;
}

int mysqlConnectionPool_init(Config * conf)
{

	int i = 0;
	pthread_mutex_init(&mutexMysqlPool, NULL);
	for (i = 0; i < MAX_MYSQL_CONNECTION; i++)
	{
		memset(&mysqlBlock[i], 0, sizeof(MysqlBlock));
		mysql_init(&(mysqlBlock[i].sql));
		mysqlBlock[i].conn = mysql_real_connect(&(mysqlBlock[i].sql), conf->mysqlIP, conf->mysqlAccount, conf->mysqlPassword, conf->databaseName, 0, 0, 0);
		if (mysqlBlock[i].conn == NULL)
		{
			LOG("connection pool  init connection fail errcode:%s\n", mysql_error(&(mysqlBlock[i].sql)));
			return ERR_DATABASE_CONNECT_FAIL;
		}
		mysqlBlock[i].lastUseTime = time(NULL);
	}
	LOG("[%d] connection create success!!\n", MAX_MYSQL_CONNECTION);
	pthread_t pid;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&pid, &attr, check_connection, (void *)conf);
	pthread_attr_destroy(&attr);
	return 0;
}
void* check_connection(void* Param)
{
	int i = 0;
	Config * conf = (Config *)Param;
	LOG("mysql connection pool check thread startup success!!\n");
	while (1)
	{
		for (i = 0; i < MAX_MYSQL_CONNECTION; i++)
		{
			pthread_mutex_lock(&mutexMysqlPool);
			if (mysqlBlock[i].useFlag == 0 && ((time(NULL) - mysqlBlock[i].lastUseTime) > conf->mysqlConnectionTimeout || testConnection(mysqlBlock[i].conn) != 0))
			{

				LOG("in check_connection thread , no.[%d] connection timeout ,need reconnect\n", i);
				mysql_close(mysqlBlock[i].conn);
				memset(&mysqlBlock[i], 0, sizeof(MysqlBlock));
				mysql_init(&(mysqlBlock[i].sql));
				mysqlBlock[i].conn = mysql_real_connect(&(mysqlBlock[i].sql), conf->mysqlIP, conf->mysqlAccount, conf->mysqlPassword, conf->databaseName, 0, 0, 0);
				if (mysqlBlock[i].conn == NULL)
					LOG("warning !! in check_connection thread ,init connection fail errcode:%s\n", mysql_error(&(mysqlBlock[i].sql)));
				mysqlBlock[i].lastUseTime = time(NULL);
			}
			pthread_mutex_unlock(&mutexMysqlPool);
		}
		SLEEP(600);//十分钟刷新一次
	}
}

int mysqlConnectionPool_close(void)
{

	int i = 0;
	for (i = 0; i < MAX_MYSQL_CONNECTION; i++)
	{
		mysql_close(mysqlBlock[i].conn);
		memset(&mysqlBlock[i], 0, sizeof(MysqlBlock));
	}
	return 0;
}

MYSQL * mysqlConnectionPool_GetOneConn(void)
{
	MYSQL * ret = NULL;
	int i = 0;
	while (1)
	{
		for (i = 0; i < MAX_MYSQL_CONNECTION; i++)
		{
			pthread_mutex_lock(&mutexMysqlPool);
			if (mysqlBlock[i].useFlag == 0)
			{
				mysqlBlock[i].useFlag = 1;
				ret = mysqlBlock[i].conn;
				pthread_mutex_unlock(&mutexMysqlPool);
				//				LOG("CONNECTION POOL no.[%d] CONNECTION BE OCCUPY!!\n" , i);
				return ret;
			}
			pthread_mutex_unlock(&mutexMysqlPool);
		}
	}
}

int mysqlConnectionPool_FreeOneConn(MYSQL * conn)
{
	int ret = -22;
	int i = 0;
	for (i = 0; i < MAX_MYSQL_CONNECTION; i++)
	{
		pthread_mutex_lock(&mutexMysqlPool);
		if (mysqlBlock[i].conn == conn)
		{
			mysqlBlock[i].useFlag = 0;
			mysqlBlock[i].lastUseTime = time(NULL);
			pthread_mutex_unlock(&mutexMysqlPool);
			ret = 0;
			//			LOG("CONNECTION POOL no.[%d] CONNECTION BE RETURN!!\n" , i);
			break;
		}
		pthread_mutex_unlock(&mutexMysqlPool);
	}
	return ret;
}

