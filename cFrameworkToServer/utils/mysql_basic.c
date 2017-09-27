#include"mysql_basic.h"
#include"log.h"
//数据库查询无结果返回，只返回查询到的条数
int query_db(const char *SQL, MYSQL *connection)//执行有返回数据集的SQL语句  
{
	MYSQL_RES *result = NULL;
	int ret = -1;
	int state = mysql_query(connection, SQL);//执行SQL语句  
	if (state != 0)
	{
		DEBUG("%s\n", mysql_error(connection));
		ret = -1;
		goto end;//执行失败，返回-1  
	}
	result = mysql_store_result(connection);//得到查询结果  
	if (result == (MYSQL_RES *)NULL)
	{
		DEBUG("%s\n", mysql_error(connection));
		ret = -2;
		goto end;//执行失败，返回-1  
	}
	ret = result->row_count;
end:if (result != NULL)
{
	mysql_free_result(result);
	result = NULL;
}
	return ret;
}
//数据库执行，返回受影响的记录条数
int exec_db(const char *SQL, MYSQL *connection)//执行没有返回数据集的SQL语句  
{
	int state = mysql_query(connection, SQL);//执行SQL语句  
	if (state != 0)
	{
		DEBUG("%s\n", mysql_error(connection));
		return -1;
	}
	int count = mysql_affected_rows(connection);
	//	DEBUG("query is ok, %u rows affected\n", count);
	return count;
}
//数据库查询有结果返回，返回result
MYSQL_RES * query_db_return_result(const char *SQL, MYSQL *connection)//执行有返回数据集的SQL语句  
{
	MYSQL_RES *result = NULL;
	int ret = -1;
	int state = mysql_query(connection, SQL);//执行SQL语句  
	if (state != 0)
	{
		DEBUG("%s\n", mysql_error(connection));
		return NULL;
	}
	result = mysql_store_result(connection);//得到查询结果  
	if (result == (MYSQL_RES *)NULL)
	{
		DEBUG("%s\n", mysql_error(connection));
	}
	return result;
}