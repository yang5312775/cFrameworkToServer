#ifndef _CACHE_H_
#define _CACHE_H_
#include<basic.h>

#define CACHE_TOKEN	1
#define CACHE_NICK	2
#define CACHE_COMM	3

/**********************���ݻ���ӿ�******************************/
//����������
void createCache(void);
//�򻺳����в���һ����¼
int cacheInsert(int cache, char * key, void * obj);
//�ӻ������в�ѯһ����¼
void * cacheQuery(int cache, char * key);
//�޸Ļ�������һ����¼��ֵ
int cacheModify(int cache, char * key, void * obj);
//�ӻ�������ɾ��һ����¼
int cacheDelete(int cache, char * key);
/**********************���ݻ���ӿ�******************************/




#endif