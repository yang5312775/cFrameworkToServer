#include"mysql_connection_pool.h"

sqlConfig sqlC;
MysqlBlock *mysqlBlock = NULL;
pthread_mutex_t mutexMysqlPool;
pthread_t g_mysql_conn_pid;
int g_mysql_conn_enable = 0;

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

int mysqlConnectionPoolInit(char * mysqlIP , char * mysqlPort , char * mysqlAccount , char * mysqlPassword , char * databaseName , char * mysqlConnectionTimeout , char * connectCount)
{
	int i = 0;
	memset(&sqlC , 0 , sizeof(sqlConfig));
	strcpy(sqlC.ip , mysqlIP);
	strcpy(sqlC.account, mysqlAccount);
	strcpy(sqlC.password, mysqlPassword);
	strcpy(sqlC.databaseName, databaseName);
	sqlC.port = atoi(mysqlPort);
	sqlC.connectionTimeout = atoi(mysqlConnectionTimeout);
	sqlC.connectCount = atoi(connectCount);

	mysqlBlock = (MysqlBlock *)MALLOC(sizeof(MysqlBlock) * sqlC.connectCount);

	pthread_mutex_init(&mutexMysqlPool, NULL);
	for (i = 0; i < sqlC.connectCount; i++)
	{
		mysql_init(&(mysqlBlock[i].sql));
		mysqlBlock[i].conn = mysql_real_connect(&(mysqlBlock[i].sql), sqlC.ip, sqlC.account, sqlC.password, sqlC.databaseName, sqlC.port, 0, 0);
		if (mysqlBlock[i].conn == NULL)
		{
			log_print(L_ERROR , "connection pool  init connection fail errcode:%s\n", mysql_error(&(mysqlBlock[i].sql)));
			return ERR_DATABASE_CONNECT_FAIL;
		}
		mysqlBlock[i].lastUseTime = time(NULL);
	}
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&g_mysql_conn_pid, &attr, check_connection, NULL);
	pthread_attr_destroy(&attr);
	g_mysql_conn_enable = 1;
	return 0;
}

int mysqlConnectionPoolUnInit(void)
{

	for (int i = 0; i < sqlC.connectCount; i++)
	{
		mysql_close(mysqlBlock[i].conn);
	}
	FREE(mysqlBlock);
	mysqlBlock = NULL;
	pthread_cancel(g_mysql_conn_pid);
	SLEEP(1);
	pthread_mutex_lock(&mutexMysqlPool);
	pthread_mutex_destroy(&mutexMysqlPool);
	g_mysql_conn_enable = 0;
	return 0;
}

void* check_connection(void* Param)
{
	int i = 0;
	int count = 0;
	log_print(L_INFO , "mysql connection pool check thread startup success!!\n");
	while (1)
	{
		count++;
		if(count %(60*10) == 0)//累计600次也就是大约600秒进入for循环一次
			for (i = 0; i < sqlC.connectCount; i++)
			{
				pthread_mutex_lock(&mutexMysqlPool);
				if (mysqlBlock[i].useFlag == 0 && ((time(NULL) - mysqlBlock[i].lastUseTime) > sqlC.connectionTimeout || testConnection(mysqlBlock[i].conn) != 0))
				{

					log_print(L_WARNING  ,"in check_connection thread , no.[%d] connection timeout ,need reconnect\n", i);
					mysql_close(mysqlBlock[i].conn);
					memset(&mysqlBlock[i], 0, sizeof(MysqlBlock));
					mysql_init(&(mysqlBlock[i].sql));
					mysqlBlock[i].conn = mysql_real_connect(&(mysqlBlock[i].sql), sqlC.ip, sqlC.account, sqlC.password, sqlC.databaseName, sqlC.port, 0, 0);
					if (mysqlBlock[i].conn == NULL)
						log_print(L_WARNING  , "warning !! in check_connection thread ,init connection fail errcode:%s\n", mysql_error(&(mysqlBlock[i].sql)));
					mysqlBlock[i].lastUseTime = time(NULL);
				}
				pthread_mutex_unlock(&mutexMysqlPool);
			}
		SLEEP(1);
		pthread_testcancel();
	}
}

MYSQL * mysqlConnectionPool_GetOneConn(void)
{
	if (g_mysql_conn_enable == 0)
	{
		log_print(L_WARNING, "connection pool not init yet！\n");
		return NULL;
	}
	MYSQL * ret = NULL;
	int i = 0;
	while (1)
	{
		for (i = 0; i < sqlC.connectCount; i++)
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
		log_print(L_WARNING , "all connention be used , wait!!!!\n");
	}
}

int mysqlConnectionPool_FreeOneConn(MYSQL * conn)
{
	if (g_mysql_conn_enable == 0)
	{
		log_print(L_WARNING, "connection pool not init yet！\n");
		return -1;
	}
	int ret = -22;
	int i = 0;
	for (i = 0; i < sqlC.connectCount; i++)
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

