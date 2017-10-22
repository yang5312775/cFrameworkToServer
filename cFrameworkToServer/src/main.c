#include<basic.h>

#define SYS_NAME "xxxxxxxxxxxxxxxxxxx"
#define VERSION	"0.1"
int main(int argc, const char * argv[])
{
//	printf("%s\n" , START_SLOGAN);
	int ret = 0;
	if (argc != 2)
	{
		printf("input argument error!!!\neg:%s /user/xxx/xxx/config.ini\npress any key to quit!!\n" , argv[0]);
		goto exit;
	}
	// 初始化socket 库操作，主要是windows与linux的区别，windows要调用一些接口初始化
	socket_start();
	// 初始化随机序列，用于随机值后续的使用
	srand_init((int32_t)time(NULL));
	//配置文件加载
	char * config_path = argv[1];
	printf("config_path:[%s]\n" , config_path);
	ret = configerInit(config_path);
	if (ret != RETURN_OK)
	{
		printf("load config file fail , errcode [%d]\n" , ret);
		goto exit;
	}
	//初始化内存池
	ret = memoryPoolInit(getConfig("memoryPoolLevel") , getConfig("memoryPoolCap") , getConfig("memoryPoolSize"));
	if (ret != RETURN_OK)
	{
		printf("memory pool init fail , errcode [%d]\n", ret);
		goto exit;
	}
	//log系统初始化，初始化成功后可用log_print进行输出
	ret = log_init(getConfig("LogPath") , atoi(getConfig("LogLevel")) , atoi(getConfig("LogSaveDays")));
	if (ret != RETURN_OK)
	{
		printf("log init fail , errcode [%d]\n", ret);
		goto exit;
	}
	//向log日志中添加欢迎头
	log_print(L_INFO, "\n\nSYSTEM[%s]  Version [%s]  Build time[%s %s]\n\n" ,SYS_NAME ,VERSION , __DATE__, __TIME__);
	//向日志文件输出配置信息
	print_config();




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
//	TIME_PRINT(EXTERN_RUN(test_list););
	TIME_PRINT(EXTERN_RUN(test_dict););
exit:	getchar();


	//配置参数模块销毁
	configerUnInit();
	//log系统销毁 为了保证log_print正常使用，一般最后调用。
	log_uninit();
	//内存池销毁
	memoryPoolUnInit();
	return 0;

}