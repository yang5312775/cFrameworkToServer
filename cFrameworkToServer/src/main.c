#include<basic.h>

#define SYS_NAME "xxxxxxxxxxxxxxxxxxx"
#define VERSION	"0.1"

#define TEST




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
	//启动线程池
	ret = threadpool_create(atoi(getConfig("ThreadpoolCount")), 0);
	if (ret != 0)
	{
		printf("thread pool init fail , errcode [%d]\n", ret);
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
	if (strcmp("1", getConfig("MysqlEnable")) == 0)
	{
		ret = mysqlConnectionPoolInit(getConfig("MysqlIP"), \
			getConfig("MysqlPort"),\
			getConfig("MysqlAccount"), \
			getConfig("MysqlPassword"), \
			getConfig("DatabaseName"),\
			getConfig("MysqlConnectionTimeout"),\
			getConfig("MysqlConnectionPoolMaxCount"));
		if (ret != RETURN_OK)
		{
			printf("mysqlConnectionPoolInit fail , errcode [%d]\n", ret);
			goto exit;
		}
	}
	log_print(L_INFO, "[%s] mysql connection create success, mysql connection pool init success!!\n", getConfig("MysqlConnectionPoolMaxCount"));

	socket_server_start(NULL , atoi(getConfig("ServerPort")) , atoi(getConfig("BackLog")));   //block here!!!





#ifdef TEST
//	TIME_PRINT(EXTERN_RUN(test_memory_pool););
//	TIME_PRINT(EXTERN_RUN(test_list););
//	TIME_PRINT(EXTERN_RUN(test_pool_event););
	
	TIME_PRINT(EXTERN_RUN(test_threadpool););
#endif


exit:	getchar();


	//数据库连接模块池销毁
	mysqlConnectionPoolUnInit();
	log_print(L_INFO, "mysql connection pool uninit success\n");
	//配置参数模块销毁
	configerUnInit();
	log_print(L_INFO, "log module uninit success\n");
	//log系统销毁 为了保证log_print正常使用，一般最后调用。
	log_print(L_INFO, "log module uninit start!\n");
	log_uninit();
	//线程池销毁
	threadpool_destroy(atoi(getConfig("ThreadpoolQuitType")));
	//内存池销毁
	memoryPoolUnInit();
	return 0;

}