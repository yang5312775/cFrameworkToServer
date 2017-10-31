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
	// ��ʼ��socket ���������Ҫ��windows��linux������windowsҪ����һЩ�ӿڳ�ʼ��
	socket_start();
	// ��ʼ��������У��������ֵ������ʹ��
	srand_init((int32_t)time(NULL));
	//�����ļ�����
	char * config_path = argv[1];
	printf("config_path:[%s]\n" , config_path);
	ret = configerInit(config_path);
	if (ret != RETURN_OK)
	{
		printf("load config file fail , errcode [%d]\n" , ret);
		goto exit;
	}
	//��ʼ���ڴ��
	ret = memoryPoolInit(getConfig("memoryPoolLevel") , getConfig("memoryPoolCap") , getConfig("memoryPoolSize"));
	if (ret != RETURN_OK)
	{
		printf("memory pool init fail , errcode [%d]\n", ret);
		goto exit;
	}
	//�����̳߳�
	ret = threadpool_create(atoi(getConfig("ThreadpoolCount")), 0);
	if (ret != 0)
	{
		printf("thread pool init fail , errcode [%d]\n", ret);
		goto exit;
	}
	//logϵͳ��ʼ������ʼ���ɹ������log_print�������
	ret = log_init(getConfig("LogPath") , atoi(getConfig("LogLevel")) , atoi(getConfig("LogSaveDays")));
	if (ret != RETURN_OK)
	{
		printf("log init fail , errcode [%d]\n", ret);
		goto exit;
	}
	//��log��־����ӻ�ӭͷ
	log_print(L_INFO, "\n\nSYSTEM[%s]  Version [%s]  Build time[%s %s]\n\n" ,SYS_NAME ,VERSION , __DATE__, __TIME__);
	//����־�ļ����������Ϣ
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


	//���ݿ�����ģ�������
	mysqlConnectionPoolUnInit();
	log_print(L_INFO, "mysql connection pool uninit success\n");
	//���ò���ģ������
	configerUnInit();
	log_print(L_INFO, "log module uninit success\n");
	//logϵͳ���� Ϊ�˱�֤log_print����ʹ�ã�һ�������á�
	log_print(L_INFO, "log module uninit start!\n");
	log_uninit();
	//�̳߳�����
	threadpool_destroy(atoi(getConfig("ThreadpoolQuitType")));
	//�ڴ������
	memoryPoolUnInit();
	return 0;

}