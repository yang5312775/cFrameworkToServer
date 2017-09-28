#include<basic.h>


int write_log(const char *format, ...);
int initLog(char * log_directory);
int closeLog(void);
int LOG(const char *format, ...);
int DEBUG(const char *format, ...);