#ifndef LOG_NEW_H
#define LOG_NEW_H
#include<basic.h>


#define L_ERROR	0
#define L_WARNING	1
#define L_INFO	2
#define	L_DEBUG	3

typedef struct {
	int level;
	char * data_body;
}log_msg;


int log_init(char * log_root_path, int log_level, int log_save_days);

int log_print(int level, const char *format, ...);

int log_uninit(void);

#endif