#include<memory_pool.h>
//
// 测试 memory_pool.h 接口的使用
//
void test_memory_pool(void) {
	printf("press any key to start init and malloc memory pool\n");
	getchar();
	unsigned int a[10] = { 8 ,16 ,32 ,64 ,128 , 256 , 512 , 1024 ,2028 , 4096};
	unsigned int b[10] = { 100 ,500 ,1000 ,1500 ,2000 , 3000 , 4000 , 5000 ,4000 , 2000};
	MALLOC_INIT(10, a , b);
	int i = 455;
#if 0
	while (i > 0)
	{
		node = buffer_malloc(6);
		if (node != NULL)
		{
			printf("pool no[%d] get memory success!!!\n" , node->column);
			node->data[0] = i;
		}
		else
		{
			printf("malloc fail !!!\n");
			getchar();
		}
		i--;
		Sleep(10);
	}
	getchar();
#endif
	void * node1 = MALLOC(6);
	void * node2 = MALLOC(172);
	void * node3 = MALLOC(186);
	void * node4 = MALLOC(1300);
	void * node5 = MALLOC(2100);
	MALLOC(40);
	MALLOC(60);
	MALLOC(80);
	MALLOC(100);
	MALLOC(120);
	MALLOC(130);
	MALLOC(150);
	MALLOC(180);
	MALLOC(700);
	MALLOC(900);
	memoryPoolPrint();
	printf("press any key to free memory pool\n");
	getchar();
	FREE(node5);
	FREE(node4);
	FREE(node3);
	memoryPoolPrint();
	FREE(node2);
	FREE(node1);
	memoryPoolPrint();
	printf("press any key to destory memory pool\n");
	getchar();
	memoryPoolUnInit();
	printf("press any key to quit\n");
	getchar();
}