#include"log.h"

char ga_str_log_level[][10] = { "ERROR",
"WARNING",
"INFO ",
"DEBUG"
};
int g_log_enable = 0;
FILE * g_logFile = NULL;
char g_logPath[512] = { 0 };
char g_log_name[64] = { 0 };
int g_log_level = 3;
int	g_oldDay = 0;
int g_log_save_days = 30;


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
	log_msg * msg = Param;
	int ret = -1;
	char path[512] = { 0 };
	char name_new[64] = { 0 };
	int i = 0;
	if (update_logname(name_new) == 1)
	{
		memset(path, 0, 512);
		strcat(path, g_logPath);
		strcat(path, g_log_name);
		fflush(g_logFile);
		fclose(g_logFile);
		g_logFile = NULL;
		memset(path, 0, 512);
		strcat(path, g_logPath);
		strcat(path, name_new);
		strcpy(g_log_name , name_new);
		g_logFile = fopen(path, "a+");
		if (g_logFile == NULL)
		{
			printf("log name  [%s] open fail\n" , g_log_name);
			return;
		}
	}
	if (msg->level > g_log_level)
		return;
	if (msg->data_body == NULL)
		return;
	fwrite(msg->data_body, 1, strlen(msg->data_body), g_logFile);
	fflush(g_logFile);
	FREE(msg->data_body);
	FREE(msg);
}

int log_init(char * log_root_path, int log_level, int log_save_days)
{
	char path[512] = { 0 };
	if (log_root_path == NULL)
		return ERR_LOG_PATH_INVALID;
	strcpy(g_logPath, log_root_path);
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
	if (log_save_days < 0 || log_save_days >365)
		return ERR_LOG_SAVE_DAYS_INVALID;
	if (log_save_days != 0)
		g_log_save_days = log_save_days;

	g_log_enable = 1;
	return 0;
}

int log_uninit(void)
{
	g_log_enable = 0;
	return 0;
}

int log_print(int level, const char *format, ...)
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
	vsprintf(buffer2, format, arg);
	va_end(arg);
	strcat(buffer, buffer2);
	FREE(buffer2);
	log_msg * msg = MALLOC(sizeof(log_msg));
	msg->level = level;
	msg->data_body = buffer;
	threadpool_add(log_handle, msg, 0);
	return 0;
}


