#include<list.h>


typedef struct {
	int a;
	int b;
	int c;
	int d;
}test;

//
// 测试 list.h 接口的使用
//
void test_list(void) {

	printf("test test_list start!!!\n");
	time_t oldTime = time(0);
	list * list1 = listCreate();
	char * data1 = "abcdefg";
	char * data2 = "abcdeasdfasdffg";
	char * data3 = "abcdsadfasdfasdfasdfaefg";
	test aa;
	aa.a = 12;
	aa.b = 15;
	aa.c = 18;
	aa.d = 10;
	listAddNodeToHead(list1, &aa);
	listAddNodeToHead(list1 , data1);
	listAddNodeToHead(list1, data2);
	listAddNodeToHead(list1, data3);
	void * retVal = listPopNodeFromTail(list1);
	test * aaT = retVal;
	retVal = listPopNodeFromHead(list1);
	char * ssss = retVal;

	//	Sleep(100);
	printf("test test_list end!!!\n");
}