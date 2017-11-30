#ifndef _H_MYSQL_BASIC
#define _H_MYSQL_BASIC
#include<basic.h>
											
int exec_db(const char *SQL, MYSQL *connection);//执行没有返回数据集的SQL语句  
												
MYSQL_RES * query_db(const char *SQL, MYSQL *connection);//执行有返回数据集的SQL语句  

#endif
