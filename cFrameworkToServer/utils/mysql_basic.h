#ifndef _H_MYSQL_BASIC
#define _H_MYSQL_BASIC
#include<basic.h>

//数据库查询无结果返回，只返回查询到的条数
int query_db(const char *SQL, MYSQL *connection);//执行有返回数据集的SQL语句  
												 //数据库执行，返回受影响的记录条数
int exec_db(const char *SQL, MYSQL *connection);//执行没有返回数据集的SQL语句  
												//数据库查询有结果返回，返回result
MYSQL_RES * query_db_return_result(const char *SQL, MYSQL *connection);//执行有返回数据集的SQL语句  

#endif
