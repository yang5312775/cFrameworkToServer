#ifndef config_h
#define config_h
#include<basic.h>

//×°ÔØconfigÎÄ¼ş
int loadConfig(char * configFilePath);

char * getConfig(char * key);

int setConfig(char * key, char * value);

int addConfig(char * key, char * value);
#endif