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
* ��Ȩ���� (C)2015, Zhou Zhaoxiong��
*
* �ļ����ƣ�WriteLog.c
* �ļ���ʶ����
* ����ժҪ����ʾ��־��Ϣ�Ĵ�ӡ����
* ����˵������
* ��ǰ�汾��V1.0
* ��   �ߣ�Zhou Zhaoxiong
* ������ڣ�20150522
*
**********************************************************************/
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

// �ض�����������
typedef signed   int    INT32;
typedef unsigned int    UINT32;
typedef unsigned char   UINT8;

// �����궨��
#define WRITELOGFILE(level, msg)  WriteLogFile(__FILE__, __FUNCTION__, __LINE__, level, msg)

// ȫ�ֱ���
UINT32  g_iLogLevel = 0;    // ��־�ȼ�
UINT32  g_iLogPosition = 0;    // ��־λ��
UINT8   g_szLogFile[100] = { 0 };  // ��·������־�ļ���

								   // ��־������
#define LOG_FATAL       0     // ���ش���
#define LOG_ERROR       1     // һ�����
#define LOG_WARN        2     // ����
#define LOG_INFO        3     // һ����Ϣ
#define LOG_TRACE       4     // ������Ϣ
#define LOG_DEBUG       5     // ������Ϣ
#define LOG_ALL         6     // ������Ϣ

								   // Ա����Ϣ�ṹ�嶨��
typedef struct
{
	UINT8  szEmployeeName[128];    // Ա������
	INT32  iEmployeeAge;           // Ա������
} T_EmployeeInfo;

// ��������
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
* ����������������
* �����������
* �����������
* �� �� ֵ����
* ����˵������
* �޸�����        �汾��       �޸���            �޸�����
* -------------------------------------------------------------------
* 20150522        V1.0     Zhou Zhaoxiong        ����
***********************************************************************/
INT32 main()
{
	UINT8  szLogContent[1024] = { 0 };

	// ��ȡ�����ļ��и����������ֵ
	GetConfigValue();

	// �ȴ�ӡ�汾�����Ϣ
	snprintf(szLogContent, sizeof(szLogContent) - 1, "Version [1.0], Build time[%s %s].", __DATE__, __TIME__);
	WRITELOGFILE(LOG_INFO, szLogContent);

	// ��ӡ��һ����־
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Fatal log info!");
	WRITELOGFILE(LOG_FATAL, szLogContent);

	// ��ӡ�ڶ�����־
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Error log info!");
	WRITELOGFILE(LOG_ERROR, szLogContent);

	// ��ӡ��������־
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Warn log info!");
	WRITELOGFILE(LOG_WARN, szLogContent);

	// ��ӡ��������־
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Info log info!");
	WRITELOGFILE(LOG_INFO, szLogContent);

	// ��ӡ��������־
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Trace log info!");
	WRITELOGFILE(LOG_TRACE, szLogContent);

	// ��ӡ��������־
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The Debug log info!");
	WRITELOGFILE(LOG_DEBUG, szLogContent);

	// ��ӡ��������־
	snprintf(szLogContent, sizeof(szLogContent) - 1, "The All log info!");
	WRITELOGFILE(LOG_ALL, szLogContent);

	GetEmployeeInfo();   // ��ȡ����ӡԱ����Ϣ

	return 0;
}


/**********************************************************************
* ���������� ��ȡ����ӡԱ����Ϣ
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��         �޸���         �޸�����
* -------------------------------------------------------------------
* 20150522        V1.0      Zhou Zhaoxiong      ����
********************************************************************/
void GetEmployeeInfo()
{
	UINT8  szLogContent[1024] = { 0 };

	T_EmployeeInfo tEmployeeInfo = { 0 };

	// ��ȡ����ӡԱ����Ϣ
	// ��ȡԱ������
	GetConfigFileStringValue("EMPLOYEEINFO", "EmployeeName", "", tEmployeeInfo.szEmployeeName, sizeof(tEmployeeInfo.szEmployeeName), "Config.ini");

	// ��ȡԱ������
	tEmployeeInfo.iEmployeeAge = GetConfigFileIntValue("EMPLOYEEINFO", "EmployeeAge", 0, "Config.ini");
	if (tEmployeeInfo.iEmployeeAge == -1)  // �жϻ�ȡ���������Ƿ���ȷ
	{
		snprintf(szLogContent, sizeof(szLogContent) - 1, "GetEmployeeInfo: Get EmployeeAge failed!");
		WRITELOGFILE(LOG_ERROR, szLogContent);
		return;
	}

	// ��ӡ��ȡ����Ա������������
	snprintf(szLogContent, sizeof(szLogContent) - 1, "EmployeeName is %s, EmployeeAge is %d", tEmployeeInfo.szEmployeeName, tEmployeeInfo.iEmployeeAge);
	WRITELOGFILE(LOG_INFO, szLogContent);
}

/**********************************************************************
* ���������� ������д����־�ļ���
* ��������� pszFileName-�����ļ���
pszFunctionName-�������ں�����
iCodeLine-������
iLogLevel-��־�ȼ�
pszContent-ÿ����־�ľ�������
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��         �޸���          �޸�����
* -------------------------------------------------------------------
* 20150522        V1.0      Zhou Zhaoxiong      ����
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

	// ������־�ȼ�
	if (iLogLevel > g_iLogLevel)
	{
		return;
	}

	fp = fopen(g_szLogFile, "at+");      // ���ļ�, ÿ��д���ʱ���ں���׷��
	if (fp == NULL)
	{
		return;
	}

	// д����־ʱ��
	GetTime(szTimeStr);
	fputs(szTimeStr, fp);

	// д����־����
	if (g_iLogPosition == 1)    // ����־��Ϣ����ʾ"�ļ���/������/��������"��Ϣ
	{
		snprintf(szLogContent, sizeof(szLogContent) - 1, "[%s][%s][%04d][%s]%s\n", pszFileName, pszFunctionName, iCodeLine, LogLevel(iLogLevel), pszContent);
	}
	else    // ��������־��Ϣ����ʾ"�ļ���/��������"��Ϣ
	{
		snprintf(szLogContent, sizeof(szLogContent) - 1, "[%s]%s\n", LogLevel(iLogLevel), pszContent);
	}
	fputs(szLogContent, fp);

	fflush(fp);     // ˢ���ļ�
	fclose(fp);     // �ر��ļ�
	fp = NULL;      // ���ļ�ָ����Ϊ��

	return;
}


/**********************************************************************
* ���������� ��ȡ��Ӧ����־�ȼ�
* ��������� iLogLevel-��־�ȼ�
* ��������� ��
* �� �� ֵ�� ��־�ȼ���Ϣ�ַ���
* ����˵���� ��
* �޸�����        �汾��         �޸���          �޸�����
* -------------------------------------------------------------------
* 20150522        V1.0      Zhou Zhaoxiong      ����
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
* ���������� ��ȡʱ�䴮
* ��������� pszTimeStr-ʱ�䴮
* ��������� pszTimeStr-ʱ�䴮
* �� �� ֵ�� ��
* ����˵���� ʱ�䴮��ʽ: YYYY.MM.DD HH:MIN:SS.Usec
* �޸�����        �汾��         �޸���         �޸�����
* -------------------------------------------------------------------
* 20150522        V1.0      Zhou Zhaoxiong      ����
********************************************************************/
void GetTime(UINT8 *pszTimeStr)
{
	struct tm      tSysTime = { 0 };
	struct timeval tTimeVal = { 0 };
	time_t         tCurrentTime = { 0 };

	UINT8  szUsec[20] = { 0 };    // ΢��
	UINT8  szMsec[20] = { 0 };    // ����

	if (pszTimeStr == NULL)
	{
		return;
	}

	tCurrentTime = time(NULL);
	localtime_r(&tCurrentTime, &tSysTime);   // localtime_r���̰߳�ȫ��

	gettimeofday(&tTimeVal, NULL);
	sprintf(szUsec, "%06d", tTimeVal.tv_usec);  // ��ȡ΢��
	strncpy(szMsec, szUsec, 3);                // ΢���ǰ3λΪ����(1����=1000΢��)

	sprintf(pszTimeStr, "[%04d.%02d.%02d %02d:%02d:%02d.%3.3s]",
		tSysTime.tm_year + 1900, tSysTime.tm_mon + 1, tSysTime.tm_mday,
		tSysTime.tm_hour, tSysTime.tm_min, tSysTime.tm_sec, szMsec);
}


/**********************************************************************
* ���������� ��ȡ�����ļ�����·��(�����ļ���)
* ��������� pszConfigFileName-�����ļ���
pszWholePath-�����ļ�����·��(�����ļ���)
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����       �汾��        �޸���        �޸�����
* ------------------------------------------------------------------
* 20150522       V1.0     Zhou Zhaoxiong     ����
********************************************************************/
void GetCompletePath(UINT8 *pszConfigFileName, UINT8 *pszWholePath)
{
	UINT8 *pszHomePath = NULL;
	UINT8  szWholePath[256] = { 0 };

	// �ȶ�������������쳣�ж�
	if (pszConfigFileName == NULL || pszWholePath == NULL)
	{
		return;
	}

	pszHomePath = (UINT8 *)getenv("HOME");     // ��ȡ��ǰ�û����ڵ�·��
	if (pszHomePath == NULL)
	{
		return;
	}

	// ƴװ�����ļ�·��
	snprintf(szWholePath, sizeof(szWholePath) - 1, "%s/zhouzx/test/etc/%s", pszHomePath, pszConfigFileName);

	strncpy(pszWholePath, szWholePath, strlen(szWholePath));
}


/**********************************************************************
* ���������� ��ȡ��־�������ֵ
* ��������� ��
* ��������� ��
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����        �汾��         �޸���         �޸�����
* -------------------------------------------------------------------
* 20150522        V1.0      Zhou Zhaoxiong      ����
********************************************************************/
void GetConfigValue()
{
	UINT8  szLogDir[256] = { 0 };

	// ��־�ȼ�
	g_iLogLevel = GetConfigFileIntValue("LOG", "LogLevel", 5, "Config.ini");

	// ��־λ��
	g_iLogPosition = GetConfigFileIntValue("LOG", "LogPosition", 0, "Config.ini");

	// ��־�ļ����Ŀ¼
	GetConfigFileStringValue("LOG", "LogDir", "", szLogDir, sizeof(szLogDir), "Config.ini");
	snprintf(g_szLogFile, sizeof(g_szLogFile) - 1, "%s/WriteLog.log", szLogDir);
}


/**********************************************************************
* ���������� ��ȡ������ַ���ֵ
* ��������� fp-�����ļ�ָ��
pszSectionName-����, ��: GENERAL
pszKeyName-��������, ��: EmployeeName
iOutputLen-������泤��
* ��������� pszOutput-�������
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����       �汾��        �޸���         �޸�����
* ------------------------------------------------------------------
* 20150522       V1.0     Zhou Zhaoxiong     ����
********************************************************************/
void GetStringContentValue(FILE *fp, UINT8 *pszSectionName, UINT8 *pszKeyName, UINT8 *pszOutput, UINT32 iOutputLen)
{
	UINT8  szSectionName[100] = { 0 };
	UINT8  szKeyName[100] = { 0 };
	UINT8  szContentLine[256] = { 0 };
	UINT8  szContentLineBak[256] = { 0 };
	UINT32 iContentLineLen = 0;
	UINT32 iPositionFlag = 0;

	// �ȶ�������������쳣�ж�
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
		fgets(szContentLine, sizeof(szContentLine), fp);      // ��ȡ����

															  // �ж��Ƿ���ע����(��;��ͷ���о���ע����)�������������ַ���ͷ����
		if (szContentLine[0] == ';' || szContentLine[0] == '\r' || szContentLine[0] == '\n' || szContentLine[0] == '\0')
		{
			continue;
		}

		// ƥ�����
		if (strncasecmp(szSectionName, szContentLine, strlen(szSectionName)) == 0)
		{
			while (feof(fp) == 0)
			{
				memset(szContentLine, 0x00, sizeof(szContentLine));
				memset(szContentLineBak, 0x00, sizeof(szContentLineBak));
				fgets(szContentLine, sizeof(szContentLine), fp);     // ��ȡ�ֶ�ֵ

																	 // �ж��Ƿ���ע����(��;��ͷ���о���ע����)
				if (szContentLine[0] == ';')
				{
					continue;
				}

				memcpy(szContentLineBak, szContentLine, strlen(szContentLine));

				// ƥ����������
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

					iPositionFlag = iPositionFlag + 1;    // ����=��λ��

					if (iPositionFlag > iContentLineLen)
					{
						continue;
					}

					memset(szContentLine, 0x00, sizeof(szContentLine));
					strcpy(szContentLine, szContentLineBak + iPositionFlag);

					// ȥ�������е��޹��ַ�
					for (iPositionFlag = 0; iPositionFlag < strlen(szContentLine); iPositionFlag++)
					{
						if (szContentLine[iPositionFlag] == '\r' || szContentLine[iPositionFlag] == '\n' || szContentLine[iPositionFlag] == '\0')
						{
							szContentLine[iPositionFlag] = '\0';
							break;
						}
					}

					// �����������ݿ��������������
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
* ���������� �������ļ��л�ȡ�ַ���
* ��������� pszSectionName-����, ��: GENERAL
pszKeyName-��������, ��: EmployeeName
pDefaultVal-Ĭ��ֵ
iOutputLen-������泤��
pszConfigFileName-�����ļ���
* ��������� pszOutput-�������
* �� �� ֵ�� ��
* ����˵���� ��
* �޸�����       �汾��        �޸���         �޸�����
* ------------------------------------------------------------------
* 20150522       V1.0     Zhou Zhaoxiong     ����
********************************************************************/
void GetConfigFileStringValue(UINT8 *pszSectionName, UINT8 *pszKeyName, UINT8 *pDefaultVal, UINT8 *pszOutput, UINT32 iOutputLen, UINT8 *pszConfigFileName)
{
	FILE  *fp = NULL;
	UINT8  szWholePath[256] = { 0 };

	// �ȶ�������������쳣�ж�
	if (pszSectionName == NULL || pszKeyName == NULL || pszOutput == NULL || pszConfigFileName == NULL)
	{
		printf("GetConfigFileStringValue: input parameter(s) is NULL!\n");
		return;
	}

	// ��ȡĬ��ֵ
	if (pDefaultVal == NULL)
	{
		strcpy(pszOutput, "");
	}
	else
	{
		strcpy(pszOutput, pDefaultVal);
	}

	// �������ļ�
	GetCompletePath(pszConfigFileName, szWholePath);
	fp = fopen(szWholePath, "r");
	if (fp == NULL)
	{
		printf("GetConfigFileStringValue: open %s failed!\n", szWholePath);
		return;
	}

	// ���ú������ڻ�ȡ�����������ֵ
	GetStringContentValue(fp, pszSectionName, pszKeyName, pszOutput, iOutputLen);

	// �ر��ļ�
	fclose(fp);
	fp = NULL;
}


/**********************************************************************
* ���������� �������ļ��л�ȡ���ͱ���
* ��������� pszSectionName-����, ��: GENERAL
pszKeyName-��������, ��: EmployeeName
iDefaultVal-Ĭ��ֵ
pszConfigFileName-�����ļ���
* ��������� ��
* �� �� ֵ�� iGetValue-��ȡ��������ֵ   -1-��ȡʧ��
* ����˵���� ��
* �޸�����       �汾��       �޸���         �޸�����
* ------------------------------------------------------------------
* 20150522       V1.0     Zhou Zhaoxiong     ����
********************************************************************/
INT32 GetConfigFileIntValue(UINT8 *pszSectionName, UINT8 *pszKeyName, UINT32 iDefaultVal, UINT8 *pszConfigFileName)
{
	UINT8  szGetValue[512] = { 0 };
	INT32  iGetValue = 0;

	// �ȶ�������������쳣�ж�
	if (pszSectionName == NULL || pszKeyName == NULL || pszConfigFileName == NULL)
	{
		printf("GetConfigFileIntValue: input parameter(s) is NULL!\n");
		return -1;
	}

	GetConfigFileStringValue(pszSectionName, pszKeyName, NULL, szGetValue, 512 - 1, pszConfigFileName);    // �Ƚ���ȡ��ֵ������ַ��ͻ�����

	if (szGetValue[0] == '\0' || szGetValue[0] == ';')    // ����ǽ�������ֺ�, ��ʹ��Ĭ��ֵ
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