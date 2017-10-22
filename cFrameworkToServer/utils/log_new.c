#include"log_new.h"




char ga_str_log_level[][10] = { "ERROR",
"WARNING",
"INFO ",
"DEBUG"
};
int g_log_enable = 0;
list * g_log_list = NULL;
pthread_mutex_t  g_list_lock;
FILE * g_logFile = NULL;
char g_logPath[512] = { 0 };
char g_log_name[64] = { 0 };
int g_log_level = 3;
int	g_oldDay = 0;
int g_log_save_days = 30;
pthread_t g_log_pid;


int update_logname(char * logname)
{
	time_t time_log = time(NULL);
	struct tm* tm_log = localtime(&time_log);
	if (tm_log->tm_mday != g_oldDay)
	{
		sprintf(logname, "LOG%04d-%02d-%02d.log", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday);
		g_oldDay = tm_log->tm_mday;
		return 1;
	}
	return 0;
}

void* log_handle(void* Param)
{
	log_msg * msg = NULL;
	int ret = -1;
	char path[512] = { 0 };
	int i = 0;
	while (1)
	{
		pthread_testcancel();
		if (update_logname(g_log_name) == 1)
		{
			memset(path, 0, 512);
			strcat(path, g_logPath);
			strcat(path, g_log_name);
			fflush(g_logFile);
			fclose(g_logFile);
			g_logFile = NULL;
			g_logFile = fopen(path, "a+");
			if (g_logFile == NULL)
				break;
		}
		pthread_mutex_lock(&g_list_lock);
		msg = listPopNodeFromHead(g_log_list);
		pthread_mutex_unlock(&g_list_lock);
		if (msg == NULL)
			continue;
		if (msg->level > g_log_level)
			continue;
		if (msg->data_body == NULL)
			continue;
		fwrite(msg->data_body, 1, strlen(msg->data_body), g_logFile);
		fflush(g_logFile);
		FREE(msg->data_body);
		FREE(msg);
	}
	while (i++ <100)
	{
		printf("LOG FILE [%s] OPEN FAIL!!!!!!\n", path);
	}
	exit(1);
	return 0;
}

int log_init(char * log_root_path , int log_level , int log_save_days)
{
	char path[512] = { 0 }; 
	if (log_root_path == NULL)
		return ERR_LOG_PATH_INVALID;
	strcpy(g_logPath , log_root_path);
	int path_len = strlen(log_root_path);
	if (*(log_root_path + path_len) != '/')
		strcat(g_logPath, "/");
	strcat(path, g_logPath);
	update_logname(g_log_name);
	strcat(path, g_log_name);
	g_logFile = fopen(path, "a+");
	if (g_logFile == NULL)
		return ERR_CONFIG_FILE_OPEN_FAIL;
	if (log_level < L_ERROR || log_level > L_DEBUG)
		return ERR_LOG_LEVEL_INVALID;
	g_log_level = log_level;
	if(log_save_days < 0 || log_save_days >365)
		return ERR_LOG_SAVE_DAYS_INVALID;
	if (log_save_days != 0)
		g_log_save_days = log_save_days;

	g_log_list = listCreate();
	pthread_mutex_init(&g_list_lock, NULL);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_create(&g_log_pid, &attr, log_handle, NULL);
	pthread_attr_destroy(&attr);
	g_log_enable = 1;
	return 0;
}

int log_uninit(void)
{
	int length_list = 0 , ret = -1;
	do {
		length_list = listLength(g_log_list);
	} while (length_list != 0);
	listRelease(g_log_list);
	ret = pthread_cancel(g_log_pid);
	pthread_mutex_lock(&g_list_lock);
	pthread_mutex_destroy(&g_list_lock);
	fclose(g_logFile);
	g_log_enable = 0;
	return 0;
}

int log_print(int level , const char *format, ...)
{
	if (g_log_enable == 0)
		return ERR_LOG_NOT_INIT;
	va_list arg;
	char * buffer = MALLOC(500);
	char * buffer2 = MALLOC(500);
	time_t time_log = time(NULL);
	struct tm* tm_log = localtime(&time_log);
	switch (level)
	{
	case L_ERROR:
		sprintf(buffer, "[%s][%04d-%02d-%02d %02d:%02d:%02d][%s][%s][%d]", \
			ga_str_log_level[level], \
			tm_log->tm_year + 1900, \
			tm_log->tm_mon + 1, \
			tm_log->tm_mday, \
			tm_log->tm_hour, \
			tm_log->tm_min, \
			tm_log->tm_sec, \
			__FILE__, \
			__FUNCTION__, \
			__LINE__);
		break;
	case L_WARNING:
		sprintf(buffer, "[%s][%04d-%02d-%02d %02d:%02d:%02d][%s][%s][%d]", \
			ga_str_log_level[level], \
			tm_log->tm_year + 1900, \
			tm_log->tm_mon + 1, \
			tm_log->tm_mday, \
			tm_log->tm_hour, \
			tm_log->tm_min, \
			tm_log->tm_sec, \
			__FILE__, \
			__FUNCTION__, \
			__LINE__);
		break;
	case L_INFO:
		sprintf(buffer, "[%s][%04d-%02d-%02d %02d:%02d:%02d]", \
			ga_str_log_level[level], \
			tm_log->tm_year + 1900, \
			tm_log->tm_mon + 1, \
			tm_log->tm_mday, \
			tm_log->tm_hour, \
			tm_log->tm_min, \
			tm_log->tm_sec);
		break;
	case L_DEBUG:
		sprintf(buffer, "[%s][%04d-%02d-%02d %02d:%02d:%02d]", \
			ga_str_log_level[level], \
			tm_log->tm_year + 1900, \
			tm_log->tm_mon + 1, \
			tm_log->tm_mday, \
			tm_log->tm_hour, \
			tm_log->tm_min, \
			tm_log->tm_sec);
		break;
	};
	va_start(arg, format);
	vsprintf(buffer2, format , arg);
	va_end(arg);
	strcat(buffer , buffer2);
	FREE(buffer2);
	log_msg * msg = MALLOC(sizeof(log_msg));
	msg->level = level;
	msg->data_body = buffer;
	pthread_mutex_lock(&g_list_lock);
	listAddNodeToTail(g_log_list , msg);
	pthread_mutex_unlock(&g_list_lock);
	return 0;
}







#if 0
/**********************************************************************
* 版权所有 (C)2015, Zhou Zhaoxiong。
*
* 文件名称：WriteLog.c
* 文件标识：无
* 内容摘要：演示日志信息的打印方法
* 其它说明：无
* 当前版本：V1.0
* 作   者：Zhou Zhaoxiong
* 完成日期：20150522
*
**********************************************************************/
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

// 重定义数据类型
typedef signed   int    INT32;
typedef unsigned int    UINT32;
typedef unsigned char   UINT8;

// 函数宏定义
#define WRITELOGFILE(level, msg)  WriteLogFile(__FILE__, __FUNCTION__, __LINE__, level, msg)

// 全局变量
UINT32  g_iLogLevel = 0;    // 日志等级
UINT32  g_iLogPosition = 0;    // 日志位置
UINT8   g_szLogFile[100] = { 0 };  // 带路径的日志文件名

								   // 日志级别定义
#define LOG_FATAL       0     // 严重错误
#define LOG_ERROR       1     // 一般错误
#define LOG_WARN        2     // 警告
#define LOG_INFO        3     // 一般信息
#define LOG_TRACE       4     // 跟踪信息
#define LOG_DEBUG       5     // 调试信息
#define LOG_ALL         6     // 所有信息

								   // 员工信息结构体定义
typedef struct
{
	UINT8  szEmployeeName[128];    // 员工姓名
	INT32  iEmployeeAge;           // 员工年龄
} T_EmployeeInfo;

// 函数声明
void WriteLogFile(UINT8 *pszFileName, UINT8 *pszFunctionName, UINT32 iCodeLine, UINT32 iLogLevel, UINT8 *pszContent);
UINT8 *LogLevel(UINT32 iLogLevel);
void GetTime(UINT8 *pszTimeStr);
void GetConfigValue();
void GetStringContentValue(FILE *fp, UINT8 *pszSectionName, UINT8 *pszKeyName, UINT8 *pszOutput, UINT32 iOutputLen);
void GetConfigFileStringValue(UINT8 *pszSectionName, UINT8 *pszKeyName, UINT8 *pDefaultVal, UINT8 *pszOutput, UINT32 iOutputLen, UINT8 *pszConfigFileName);
INT32 GetConfigFileIntValue(UINT8 *pszSectionName, UINT8 *pszKeyName, UINT32 iDefaultVal, UINT8 *pszConfigFileName);
void GetEmployeeInfo();
INT32 main();


/**********************************************************************
* 功能描述：主函数
* 输入参数：无
* 输出参数：无
* 返 回 值：无
* 其它说明：无
* 修改日期        版本号       修改人            修改内容
* -------------------------------------------------------------------
* 20150522        V1.0     Zhou Zhaoxiong        创建
***********************************************************************/
INT32 main()
{
	UINT8  szLogContent[1024] = { 0 };

	// 获取配置文件中各个配置项的值
	GetConfigValue();

	// 先打印版本相关信息
	snprintf(szLogContent, sizeof(szLogContent) - 1, "Version [1.0], Build time[%s %s].", __DATE__, __TIME__);
	WRITELOGFILE(LOG_INFO, szLogContent);

	// 打印第一条日志
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Fatal log info!");
	WRITELOGFILE(LOG_FATAL, szLogContent);

	// 打印第二条日志
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Error log info!");
	WRITELOGFILE(LOG_ERROR, szLogContent);

	// 打印第三条日志
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Warn log info!");
	WRITELOGFILE(LOG_WARN, szLogContent);

	// 打印第四条日志
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Info log info!");
	WRITELOGFILE(LOG_INFO, szLogContent);

	// 打印第五条日志
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Trace log info!");
	WRITELOGFILE(LOG_TRACE, szLogContent);

	// 打印第六条日志
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Debug log info!");
	WRITELOGFILE(LOG_DEBUG, szLogContent);

	// 打印第七条日志
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The All log info!");
	WRITELOGFILE(LOG_ALL, szLogContent);

	GetEmployeeInfo();   // 获取并打印员工信息

	return 0;
}


/**********************************************************************
* 功能描述： 获取并打印员工信息
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期        版本号         修改人         修改内容
* -------------------------------------------------------------------
* 20150522        V1.0      Zhou Zhaoxiong      创建
********************************************************************/
void GetEmployeeInfo()
{
	UINT8  szLogContent[1024] = { 0 };

	T_EmployeeInfo tEmployeeInfo = { 0 };

	// 获取并打印员工信息
	// 获取员工姓名
	GetConfigFileStringValue("EMPLOYEEINFO", "EmployeeName", "", tEmployeeInfo.szEmployeeName, sizeof(tEmployeeInfo.szEmployeeName), "Config.ini");

	// 获取员工年龄
	tEmployeeInfo.iEmployeeAge = GetConfigFileIntValue("EMPLOYEEINFO", "EmployeeAge", 0, "Config.ini");
	if (tEmployeeInfo.iEmployeeAge == -1)  // 判断获取到的年龄是否正确
	{
		snprintf(szLogContent, sizeof(szLogContent) - 1, "GetEmployeeInfo: Get EmployeeAge failed!");
		WRITELOGFILE(LOG_ERROR, szLogContent);
		return;
	}

	// 打印读取到的员工姓名和年龄
	snprintf(szLogContent, sizeof(szLogContent) - 1, "EmployeeName is %s, EmployeeAge is %d", tEmployeeInfo.szEmployeeName, tEmployeeInfo.iEmployeeAge);
	WRITELOGFILE(LOG_INFO, szLogContent);
}

/**********************************************************************
* 功能描述： 将内容写到日志文件中
* 输入参数： pszFileName-代码文件名
pszFunctionName-代码所在函数名
iCodeLine-代码行
iLogLevel-日志等级
pszContent-每条日志的具体内容
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期        版本号         修改人          修改内容
* -------------------------------------------------------------------
* 20150522        V1.0      Zhou Zhaoxiong      创建
********************************************************************/
void WriteLogFile(UINT8 *pszFileName, UINT8 *pszFunctionName, UINT32 iCodeLine, UINT32 iLogLevel, UINT8 *pszContent)
{
	FILE  *fp = NULL;
	UINT8  szLogContent[2048] = { 0 };
	UINT8  szTimeStr[128] = { 0 };

	if (pszFileName == NULL || pszContent == NULL)
	{
		return;
	}

	// 过滤日志等级
	if (iLogLevel > g_iLogLevel)
	{
		return;
	}

	fp = fopen(g_szLogFile, "at+");      // 打开文件, 每次写入的时候在后面追加
	if (fp == NULL)
	{
		return;
	}

	// 写入日志时间
	GetTime(szTimeStr);
	fputs(szTimeStr, fp);

	// 写入日志内容
	if (g_iLogPosition == 1)    // 在日志信息中显示"文件名/函数名/代码行数"信息
	{
		snprintf(szLogContent, sizeof(szLogContent) - 1, "[%s][%s][%04d][%s]%s\n", pszFileName, pszFunctionName, iCodeLine, LogLevel(iLogLevel), pszContent);
	}
	else    // 不用在日志信息中显示"文件名/代码行数"信息
	{
		snprintf(szLogContent, sizeof(szLogContent) - 1, "[%s]%s\n", LogLevel(iLogLevel), pszContent);
	}
	fputs(szLogContent, fp);

	fflush(fp);     // 刷新文件
	fclose(fp);     // 关闭文件
	fp = NULL;      // 将文件指针置为空

	return;
}


/**********************************************************************
* 功能描述： 获取对应的日志等级
* 输入参数： iLogLevel-日志等级
* 输出参数： 无
* 返 回 值： 日志等级信息字符串
* 其它说明： 无
* 修改日期        版本号         修改人          修改内容
* -------------------------------------------------------------------
* 20150522        V1.0      Zhou Zhaoxiong      创建
********************************************************************/
UINT8 *LogLevel(UINT32 iLogLevel)
{
	switch (iLogLevel)
	{
	case LOG_FATAL:
	{
		return "FATAL";
	}

	case LOG_ERROR:
	{
		return "ERROR";
	}

	case LOG_WARN:
	{
		return "WARN";
	}

	case LOG_INFO:
	{
		return "INFO";
	}

	case LOG_TRACE:
	{
		return "TRACE";
	}

	case LOG_DEBUG:
	{
		return "DEBUG";
	}

	case LOG_ALL:
	{
		return "ALL";
	}

	default:
	{
		return "OTHER";
	}
	}
}


/**********************************************************************
* 功能描述： 获取时间串
* 输入参数： pszTimeStr-时间串
* 输出参数： pszTimeStr-时间串
* 返 回 值： 无
* 其它说明： 时间串样式: YYYY.MM.DD HH:MIN:SS.Usec
* 修改日期        版本号         修改人         修改内容
* -------------------------------------------------------------------
* 20150522        V1.0      Zhou Zhaoxiong      创建
********************************************************************/
void GetTime(UINT8 *pszTimeStr)
{
	struct tm      tSysTime = { 0 };
	struct timeval tTimeVal = { 0 };
	time_t         tCurrentTime = { 0 };

	UINT8  szUsec[20] = { 0 };    // 微秒
	UINT8  szMsec[20] = { 0 };    // 毫秒

	if (pszTimeStr == NULL)
	{
		return;
	}

	tCurrentTime = time(NULL);
	localtime_r(&tCurrentTime, &tSysTime);   // localtime_r是线程安全的

	gettimeofday(&tTimeVal, NULL);
	sprintf(szUsec, "%06d", tTimeVal.tv_usec);  // 获取微秒
	strncpy(szMsec, szUsec, 3);                // 微秒的前3位为毫秒(1毫秒=1000微秒)

	sprintf(pszTimeStr, "[%04d.%02d.%02d %02d:%02d:%02d.%3.3s]",
		tSysTime.tm_year + 1900, tSysTime.tm_mon + 1, tSysTime.tm_mday,
		tSysTime.tm_hour, tSysTime.tm_min, tSysTime.tm_sec, szMsec);
}


/**********************************************************************
* 功能描述： 获取配置文件完整路径(包含文件名)
* 输入参数： pszConfigFileName-配置文件名
pszWholePath-配置文件完整路径(包含文件名)
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期       版本号        修改人        修改内容
* ------------------------------------------------------------------
* 20150522       V1.0     Zhou Zhaoxiong     创建
********************************************************************/
void GetCompletePath(UINT8 *pszConfigFileName, UINT8 *pszWholePath)
{
	UINT8 *pszHomePath = NULL;
	UINT8  szWholePath[256] = { 0 };

	// 先对输入参数进行异常判断
	if (pszConfigFileName == NULL || pszWholePath == NULL)
	{
		return;
	}

	pszHomePath = (UINT8 *)getenv("HOME");     // 获取当前用户所在的路径
	if (pszHomePath == NULL)
	{
		return;
	}

	// 拼装配置文件路径
	snprintf(szWholePath, sizeof(szWholePath) - 1, "%s/zhouzx/test/etc/%s", pszHomePath, pszConfigFileName);

	strncpy(pszWholePath, szWholePath, strlen(szWholePath));
}


/**********************************************************************
* 功能描述： 获取日志配置项的值
* 输入参数： 无
* 输出参数： 无
* 返 回 值： 无
* 其它说明： 无
* 修改日期        版本号         修改人         修改内容
* -------------------------------------------------------------------
* 20150522        V1.0      Zhou Zhaoxiong      创建
********************************************************************/
void GetConfigValue()
{
	UINT8  szLogDir[256] = { 0 };

	// 日志等级
	g_iLogLevel = GetConfigFileIntValue("LOG", "LogLevel", 5, "Config.ini");

	// 日志位置
	g_iLogPosition = GetConfigFileIntValue("LOG", "LogPosition", 0, "Config.ini");

	// 日志文件存放目录
	GetConfigFileStringValue("LOG", "LogDir", "", szLogDir, sizeof(szLogDir), "Config.ini");
	snprintf(g_szLogFile, sizeof(g_szLogFile) - 1, "%s/WriteLog.log", szLogDir);
}


/**********************************************************************
* 功能描述： 获取具体的字符串值
* 输入参数： fp-配置文件指针
pszSectionName-段名, 如: GENERAL
pszKeyName-配置项名, 如: EmployeeName
iOutputLen-输出缓存长度
* 输出参数： pszOutput-输出缓存
* 返 回 值： 无
* 其它说明： 无
* 修改日期       版本号        修改人         修改内容
* ------------------------------------------------------------------
* 20150522       V1.0     Zhou Zhaoxiong     创建
********************************************************************/
void GetStringContentValue(FILE *fp, UINT8 *pszSectionName, UINT8 *pszKeyName, UINT8 *pszOutput, UINT32 iOutputLen)
{
	UINT8  szSectionName[100] = { 0 };
	UINT8  szKeyName[100] = { 0 };
	UINT8  szContentLine[256] = { 0 };
	UINT8  szContentLineBak[256] = { 0 };
	UINT32 iContentLineLen = 0;
	UINT32 iPositionFlag = 0;

	// 先对输入参数进行异常判断
	if (fp == NULL || pszSectionName == NULL || pszKeyName == NULL || pszOutput == NULL)
	{
		printf("GetStringContentValue: input parameter(s) is NULL!\n");
		return;
	}

	sprintf(szSectionName, "[%s]", pszSectionName);
	strcpy(szKeyName, pszKeyName);

	while (feof(fp) == 0)
	{
		memset(szContentLine, 0x00, sizeof(szContentLine));
		fgets(szContentLine, sizeof(szContentLine), fp);      // 获取段名

															  // 判断是否是注释行(以;开头的行就是注释行)或以其他特殊字符开头的行
		if (szContentLine[0] == ';' || szContentLine[0] == '\r' || szContentLine[0] == '\n' || szContentLine[0] == '\0')
		{
			continue;
		}

		// 匹配段名
		if (strncasecmp(szSectionName, szContentLine, strlen(szSectionName)) == 0)
		{
			while (feof(fp) == 0)
			{
				memset(szContentLine, 0x00, sizeof(szContentLine));
				memset(szContentLineBak, 0x00, sizeof(szContentLineBak));
				fgets(szContentLine, sizeof(szContentLine), fp);     // 获取字段值

																	 // 判断是否是注释行(以;开头的行就是注释行)
				if (szContentLine[0] == ';')
				{
					continue;
				}

				memcpy(szContentLineBak, szContentLine, strlen(szContentLine));

				// 匹配配置项名
				if (strncasecmp(szKeyName, szContentLineBak, strlen(szKeyName)) == 0)
				{
					iContentLineLen = strlen(szContentLine);
					for (iPositionFlag = strlen(szKeyName); iPositionFlag <= iContentLineLen; iPositionFlag++)
					{
						if (szContentLine[iPositionFlag] == ' ')
						{
							continue;
						}
						if (szContentLine[iPositionFlag] == '=')
						{
							break;
						}

						iPositionFlag = iContentLineLen + 1;
						break;
					}

					iPositionFlag = iPositionFlag + 1;    // 跳过=的位置

					if (iPositionFlag > iContentLineLen)
					{
						continue;
					}

					memset(szContentLine, 0x00, sizeof(szContentLine));
					strcpy(szContentLine, szContentLineBak + iPositionFlag);

					// 去掉内容中的无关字符
					for (iPositionFlag = 0; iPositionFlag < strlen(szContentLine); iPositionFlag++)
					{
						if (szContentLine[iPositionFlag] == '\r' || szContentLine[iPositionFlag] == '\n' || szContentLine[iPositionFlag] == '\0')
						{
							szContentLine[iPositionFlag] = '\0';
							break;
						}
					}

					// 将配置项内容拷贝到输出缓存中
					strncpy(pszOutput, szContentLine, iOutputLen - 1);
					break;
				}
				else if (szContentLine[0] == '[')
				{
					break;
				}
			}
			break;
		}
	}
}


/**********************************************************************
* 功能描述： 从配置文件中获取字符串
* 输入参数： pszSectionName-段名, 如: GENERAL
pszKeyName-配置项名, 如: EmployeeName
pDefaultVal-默认值
iOutputLen-输出缓存长度
pszConfigFileName-配置文件名
* 输出参数： pszOutput-输出缓存
* 返 回 值： 无
* 其它说明： 无
* 修改日期       版本号        修改人         修改内容
* ------------------------------------------------------------------
* 20150522       V1.0     Zhou Zhaoxiong     创建
********************************************************************/
void GetConfigFileStringValue(UINT8 *pszSectionName, UINT8 *pszKeyName, UINT8 *pDefaultVal, UINT8 *pszOutput, UINT32 iOutputLen, UINT8 *pszConfigFileName)
{
	FILE  *fp = NULL;
	UINT8  szWholePath[256] = { 0 };

	// 先对输入参数进行异常判断
	if (pszSectionName == NULL || pszKeyName == NULL || pszOutput == NULL || pszConfigFileName == NULL)
	{
		printf("GetConfigFileStringValue: input parameter(s) is NULL!\n");
		return;
	}

	// 获取默认值
	if (pDefaultVal == NULL)
	{
		strcpy(pszOutput, "");
	}
	else
	{
		strcpy(pszOutput, pDefaultVal);
	}

	// 打开配置文件
	GetCompletePath(pszConfigFileName, szWholePath);
	fp = fopen(szWholePath, "r");
	if (fp == NULL)
	{
		printf("GetConfigFileStringValue: open %s failed!\n", szWholePath);
		return;
	}

	// 调用函数用于获取具体配置项的值
	GetStringContentValue(fp, pszSectionName, pszKeyName, pszOutput, iOutputLen);

	// 关闭文件
	fclose(fp);
	fp = NULL;
}


/**********************************************************************
* 功能描述： 从配置文件中获取整型变量
* 输入参数： pszSectionName-段名, 如: GENERAL
pszKeyName-配置项名, 如: EmployeeName
iDefaultVal-默认值
pszConfigFileName-配置文件名
* 输出参数： 无
* 返 回 值： iGetValue-获取到的整数值   -1-获取失败
* 其它说明： 无
* 修改日期       版本号       修改人         修改内容
* ------------------------------------------------------------------
* 20150522       V1.0     Zhou Zhaoxiong     创建
********************************************************************/
INT32 GetConfigFileIntValue(UINT8 *pszSectionName, UINT8 *pszKeyName, UINT32 iDefaultVal, UINT8 *pszConfigFileName)
{
	UINT8  szGetValue[512] = { 0 };
	INT32  iGetValue = 0;

	// 先对输入参数进行异常判断
	if (pszSectionName == NULL || pszKeyName == NULL || pszConfigFileName == NULL)
	{
		printf("GetConfigFileIntValue: input parameter(s) is NULL!\n");
		return -1;
	}

	GetConfigFileStringValue(pszSectionName, pszKeyName, NULL, szGetValue, 512 - 1, pszConfigFileName);    // 先将获取的值存放在字符型缓存中

	if (szGetValue[0] == '\0' || szGetValue[0] == ';')    // 如果是结束符或分号, 则使用默认值
	{
		iGetValue = iDefaultVal;
	}
	else
	{
		iGetValue = atoi(szGetValue);
	}

	return iGetValue;
}
#endif