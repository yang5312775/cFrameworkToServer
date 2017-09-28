#include<basic.h>

int main(int argc, const char * argv[])
{
	int ret = 0;
	if (argc != 2)
	{
		printf("input argument error!!!\neg:%s /user/xxx/xxx/config.ini\npress any key to quit!!\n" , argv[0]);
		getchar();
		exit(0);
	}
	//初始化内存池
	unsigned int a[10] = { 8 ,16 ,32 ,64 ,128 , 256 , 512 , 1024 ,2028 , 4096 };
	unsigned int b[10] = { 100 ,500 ,1000 ,1500 ,2000 , 3000 , 4000 , 5000 ,4000 , 2000 };
	MALLOC_INIT(10, a, b);
	// 初始化socket 库操作，主要是windows与linux的区别，windows要调用一些接口初始化
	socket_start();
	// 初始化随机序列，用于随机值后续的使用
	srand_init((int32_t)time(NULL));



#if 0
	//load config
	ret = loadConfig(argv[1]);
	if (ret != 0)
	{
		printf("load config file fail!! press any key to quit!!!\n");
		getchar();
		exit(0);
	}
	printf("config file load success.\n");

	//init log
	ret = initLog("aaaaaaaaaaaa");
	if (ret != 0)
	{
		printf("logFile init fail ,quit !!!\n");
		getchar();
		return -1;
	}

	//	ret = mysqlConnectionPool_close();
	LOG("load config success\n");
	LOG("*****************************\n");
	LOG("logPath:[%s]\n", serverConfig.logPath);
	LOG("serverPort:[%s]\n", serverConfig.serverPort);
	LOG("mysqlIP:[%s]\n", serverConfig.mysqlIP);
	LOG("mysqlPort:[%s]\n", serverConfig.mysqlPort);
	LOG("mysqlAccount:[%s]\n", serverConfig.mysqlAccount);
	LOG("mysqlPassword:[%s]\n", serverConfig.mysqlPassword);
	LOG("databaseName:[%s]\n", serverConfig.databaseName);
	LOG("mysqlConnectionTimeout:[%d]seconds\n", serverConfig.mysqlConnectionTimeout);
	LOG("authPushPoolCount:[%d]\n", serverConfig.authPushPoolCount);
	LOG("authPushPoolCapacity:");
	for (int i = 0; i< serverConfig.authPushPoolCount; i++)
		LOG("[%d] ", serverConfig.authPushPoolCapacity[i]);
	LOG("\nauthPushPoolRateX:");
	for (int i = 0; i< serverConfig.authPushPoolCount; i++)
		LOG("[%d] ", serverConfig.authPushPoolRateX[i]);
	LOG("\nauthPushPoolBandwidth:[%d]kbps\n", serverConfig.authPushPoolBandwidth);
	LOG("multicastIP:[%s]\n", serverConfig.multicastIP);
	LOG("multicastPORT:[%s]\n", serverConfig.multicastPORT);
	LOG("*****************************\n");
	//init mysql
	ret = mysqlConnectionPool_init(&serverConfig);
	if (ret != 0)
	{
		LOG("mysql connection poll init fail\n");
		getchar();
	}
	LOG("mysql connection poll init success\n");


#endif

//	TIME_PRINT(EXTERN_RUN(test_memory_pool););
	TIME_PRINT(EXTERN_RUN(test_list););
	
	getchar();
	return 0;
}