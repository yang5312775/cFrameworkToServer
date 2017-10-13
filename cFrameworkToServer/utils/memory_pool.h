#ifndef MEMORY_POOL_H  
#define MEMORY_POOL_H  
#include<basic.h>


typedef struct {
	unsigned int total_count;
	unsigned int remain_count;
	unsigned int block_len;
	long long 	addr_start;
	long long	addr_end;
	void  **header;
}memory_pool_colume;
//�ڴ��ʼ�������ַ���������ʼ��
int memoryPoolInit(char * colume_no_str, char * block_len_str, char * block_count_str[]);
//�ڴ�س�ʼ��
int MALLOC_INIT(unsigned int colume_no, unsigned int block_len[], unsigned int block_count[]);
//�����ڴ�
void * MALLOC(unsigned int size);
//�����ڴ�
int FREE(void * buffer);
//�ڴ������
int MALLOC_UNINIT(void);
//�ڴ��״̬��ӡ
int MALLOC_PRINT(void);

#endif