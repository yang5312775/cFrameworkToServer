#ifndef config_h
#define config_h
#include<basic.h>

//װ��config�ļ�
int configerInit(char * configFilePath);

void configerUnInit(void);

char * getConfig(char * key);

int setConfig(char * key, char * value);

int addConfig(char * key, char * value);

int print_config(void);
#endif