#include<log.h>



//
// 测试 log_new.h 接口的使用
//
void test_log_new(void) {

//	log_init("d:/log" , L_DEBUG, 40);

	log_print(L_ERROR , "AAAA %s bbbbb %d  cccc %s  ddddd\n" , "string1" , 12345 , "ERROR");
	log_print(L_WARNING, "AAAA %s bbbbb %d  cccc %s  ddddd\n", "string1", 12345, "WARNING");
	log_print(L_INFO, "AAAA %s bbbbb %d  cccc %s  ddddd\n", "string1", 12345, "L_INFO");
	log_print(L_DEBUG, "AAAA %s bbbbb %d  cccc %s  ddddd\n", "string1", 12345, "L_DEBUG");
	
}