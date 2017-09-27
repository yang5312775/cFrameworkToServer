#include<basic.h>
#include"config.h"
int write_log(const char *format, ...);
int initLog(Config * conf);
int closeLog(void);
int LOG(const char *format, ...);
int DEBUG(const char *format, ...);