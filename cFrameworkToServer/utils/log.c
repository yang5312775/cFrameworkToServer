#include"log.h"

#define DEBUGER

extern FILE * logFile = NULL;


int initLog(Config * conf)
{
	char path[256];
	time_t time_log = time(NULL);
	struct tm* tm_log = localtime(&time_log);
	memset(path , 0 , 256);
	sprintf(path , "%sLOG%04d-%02d-%02d.txt" , conf->logPath , tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday);
	logFile = fopen(path, "wb+");
	if (logFile == NULL)
		return ERR_CONFIG_FILE_OPEN_FAIL;
	return 0;
}




int write_log(const char *format, ...) {
	va_list arg;
	int done;

	va_start(arg, format);
	//done = vfprintf (stdout, format, arg);

	time_t time_log = time(NULL);
	struct tm* tm_log = localtime(&time_log);
	fprintf(logFile, "%04d-%02d-%02d %02d:%02d:%02d ", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);

	done = vfprintf(logFile, format, arg);
	va_end(arg);

	fflush(logFile);
	return done;
}

int closeLog(void)
{
	fflush(logFile);
	return fclose(logFile);
}


int DEBUG(const char *format, ...)
{
#ifdef DEBUGER
	va_list argPtr;
	int     count;
	va_list arg;
	int done;
	va_start(argPtr, format);                  /*  ��ȡ�ɱ�����б�  */
	fflush(stdout);                            /*  ǿ��ˢ�����������  */
	count = vfprintf(stderr, format, argPtr);  /*  ����Ϣ�������׼�������豸  */
	va_end(argPtr);                            /*  �ɱ�����б����  */
	return 0;
#else
	return 0;
#endif
	
}

int LOG(const char *format, ...)
{
#if 1
	va_list argPtr;
	int     count;
	va_list arg;
	int done;
	va_start(argPtr, format);                  /*  ��ȡ�ɱ�����б�  */
	fflush(stdout);                            /*  ǿ��ˢ�����������  */
	count = vfprintf(stderr, format, argPtr);  /*  ����Ϣ�������׼�������豸  */
	va_end(argPtr);                            /*  �ɱ�����б����  */

	va_start(arg, format);
	//done = vfprintf (stdout, format, arg);

	time_t time_log = time(NULL);
	struct tm* tm_log = localtime(&time_log);
	fprintf(logFile, "%04d-%02d-%02d %02d:%02d:%02d ", tm_log->tm_year + 1900, tm_log->tm_mon + 1, tm_log->tm_mday, tm_log->tm_hour, tm_log->tm_min, tm_log->tm_sec);

	done = vfprintf(logFile, format, arg);
	va_end(arg);

	fflush(logFile);
	return done;
#endif
}