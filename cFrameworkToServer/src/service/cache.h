#ifndef _CACHE_H_
#define _CACHE_H_
#include<basic.h>

#define CACHE_TOKEN	1
#define CACHE_NICK	2
#define CACHE_COMM	3

/**********************数据缓存接口******************************/
//创建表缓冲区
void createCache(void);
//向缓冲区中插入一条记录
int cacheInsert(int cache, char * key, void * obj);
//从缓冲区中查询一条记录
void * cacheQuery(int cache, char * key);
//修改缓冲区中一条记录的值
int cacheModify(int cache, char * key, void * obj);
//从缓冲区中删除一条记录
int cacheDelete(int cache, char * key);
/**********************数据缓存接口******************************/




#endif