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
//内存初始化，用字符串参数初始化
int memoryPoolInit(char * colume_no_str, char * block_len_str, char * block_count_str[]);
//内存池初始化
int MALLOC_INIT(unsigned int colume_no, unsigned int block_len[], unsigned int block_count[]);
//申请内存
void * MALLOC(unsigned int size);
//回收内存
int FREE(void * buffer);
//内存池销毁
int MALLOC_UNINIT(void);
//内存池状态打印
int MALLOC_PRINT(void);

#endif