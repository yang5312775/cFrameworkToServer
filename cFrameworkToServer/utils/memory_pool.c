#include"memory_pool.h"

memory_pool_colume * b_p = NULL;
int buffer_pool_no = 0;
pthread_spinlock_t  memory_spin_lock;

int memoryPoolInit(char * colume_no_str , char * block_len_str ,char * block_count_str)
{
	int ret = -1;
	int colume_no = atoi(colume_no_str);
	char * block_len_str_t = strdup(block_len_str);
	char * block_count_str_t = strdup(block_count_str);
	unsigned int * block_len = malloc(sizeof(unsigned int) * colume_no);
	unsigned int * block_count = malloc(sizeof(unsigned int) * colume_no);
	*strchr(block_len_str_t++, ']') = '\0';
	*strchr(block_count_str_t++, ']') = '\0';
	char* delim = ",";
	char* p = strtok(block_len_str_t, delim);
	for (int i = 0; i < colume_no; i++)
	{
		block_len[i] = atoi(p);
		p = strtok(NULL, delim);
	}
	p = strtok(block_count_str_t, delim);
	for (int i = 0; i < colume_no; i++)
	{
		block_count[i] = atoi(p);
		p = strtok(NULL, delim);
	}
	ret = MALLOC_INIT(colume_no , block_len , block_count);
	free(block_len);
	free(block_count);
	free(--block_len_str_t);
	free(--block_count_str_t);
	return 0;
}


int MALLOC_INIT(unsigned int colume_no, unsigned int block_len[], unsigned int block_count[])
{
	b_p = (memory_pool_colume *)malloc(sizeof(memory_pool_colume) * colume_no);
	if (b_p == NULL)
		return ERR_MEMORY_POOL_MALLOC_FAIL;
	memset(b_p, 0, sizeof(memory_pool_colume) * colume_no);
	buffer_pool_no = colume_no;
	for (int i = 0; i < colume_no; i++)
	{
		b_p[i].block_len = block_len[i];
		b_p[i].total_count = block_count[i];
		b_p[i].remain_count = block_count[i];
		b_p[i].addr_start = malloc(b_p[i].block_len * b_p[i].total_count);
		memset(b_p[i].addr_start , 0 , b_p[i].block_len * b_p[i].total_count);
		b_p[i].addr_end = b_p[i].addr_start + b_p[i].block_len * b_p[i].total_count -1;
		b_p[i].header = malloc(sizeof(void *) * b_p[i].total_count);
		for (int j = 0; j < block_count[i]; j++)
		{
			b_p[i].header[j] = b_p[i].addr_start + j * b_p[i].block_len;
		}
	}
	pthread_spin_init(&memory_spin_lock, PTHREAD_PROCESS_PRIVATE);
	return RETURN_OK;
}

void * MALLOC(unsigned int size)
{
	void * buffer = NULL;
	if (size > b_p[buffer_pool_no - 1].block_len)
	{
		printf("malloc size[%d] so big ,need new from stack!!\n", size);
a:		buffer = malloc(size);
		memset(buffer, 0, size);
		if (buffer == NULL)
			return NULL;
		return buffer;
	}
	for (int i = 0; i < buffer_pool_no; i++)
	{
		if (size > b_p[i].block_len)
			continue;
//		if (size > 8 && size <= 16)
//			printf(".");
		pthread_spin_lock(&memory_spin_lock);
		if (b_p[i].remain_count == 0)
		{
 			printf("warning!!!!  size[%d]pool use up!!!! \n", b_p[i].block_len);
			pthread_spin_unlock(&memory_spin_lock);
			continue;
		}
		buffer = b_p[i].header[b_p[i].remain_count-1];
		b_p[i].remain_count--;
		pthread_spin_unlock(&memory_spin_lock);
		memset(buffer , 0 , b_p[i].block_len);
		return buffer;
	}
	printf("warning!!!!  all of pool used up , need new from stack!!!!!! \n");
	goto a;
}

int FREE(void * buffer)
{
	int flag = 0;
	for (int i = 0; i < buffer_pool_no; i++)
	{
		if (buffer > b_p[i].addr_start && buffer < b_p[i].addr_end)
		{
//			if(b_p[i].block_len == 16)
//				printf("*");
			pthread_spin_lock(&memory_spin_lock);
			b_p[i].header[b_p[i].remain_count++] = buffer;
			pthread_spin_unlock(&memory_spin_lock);
			flag = 1;
			break;
		}	
	}
	if (flag == 0)
		free(buffer);
	return RETURN_OK;
}

int memoryPoolUnInit(void)
{
	if (b_p == NULL)
		return ERR_MEMORY_POOL_NOT_INIT;
	for (int i = 0; i < buffer_pool_no; i++)
	{
		free(b_p[i].addr_start);
		free(b_p[i].header);
	}
	free(b_p);
	b_p = NULL;
	pthread_spin_lock(&memory_spin_lock);
	pthread_spin_destroy(&memory_spin_lock);
	buffer_pool_no = 0;
	return RETURN_OK;
}

int memoryPoolPrint(void)
{
	if (b_p == NULL)
	{
		printf("buffer pool not init yet!!!\n");
		return ERR_MEMORY_POOL_NOT_INIT;
	}
	printf("\n*********************** memory pool runtime report start************************\n");
	for (int i = 0; i < buffer_pool_no; i++)
	{
		printf("pool no[%d] blocksize[%d] blockTotalCount[%d] used count[%d] used percentage[%d%%]\n" \
			, i, b_p[i].block_len, b_p[i].total_count, b_p[i].total_count - b_p[i].remain_count, (b_p[i].total_count - b_p[i].remain_count) * 100/ b_p[i].total_count);
	}
	printf("*********************** memory pool runtime report end**************************\n");
	return RETURN_OK;
}
