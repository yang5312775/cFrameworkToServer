#include"mysql_basic.h"
#include"log.h"
//数据库执行，返回受影响的记录条数
int exec_db(const char *SQL, MYSQL *connection)//执行没有返回数据集的SQL语句  
{
	int state = mysql_query(connection, SQL);//执行SQL语句  
	if (state != 0)
	{
		log_print(L_ERROR ,"%s\n", mysql_error(connection));
		printf("%s\n", mysql_error(connection));
		return -1;
	}
	int count = mysql_affected_rows(connection);
	//	log_print("query is ok, %u rows affected\n", count);
	return count;
}
//数据库查询有结果返回，返回result
MYSQL_RES * query_db(const char *SQL, MYSQL *connection)//执行有返回数据集的SQL语句  
{
	MYSQL_RES *result = NULL;
	int ret = -1;
	int state = mysql_query(connection, SQL);//执行SQL语句  
	if (state != 0)
	{
		log_print(L_ERROR ,"%s\n", mysql_error(connection));
		printf("%s\n", mysql_error(connection));
		return NULL;
	}
	result = mysql_store_result(connection);//得到查询结果  
	if (result == (MYSQL_RES *)NULL)
	{
		log_print(L_ERROR ,"%s\n", mysql_error(connection));
		printf("%s\n", mysql_error(connection));
	}
	return result;
}