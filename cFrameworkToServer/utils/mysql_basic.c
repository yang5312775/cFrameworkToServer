#include"mysql_basic.h"
#include"log.h"
//���ݿ��ѯ�޽�����أ�ֻ���ز�ѯ��������
int query_db(const char *SQL, MYSQL *connection)//ִ���з������ݼ���SQL���  
{
	MYSQL_RES *result = NULL;
	int ret = -1;
	int state = mysql_query(connection, SQL);//ִ��SQL���  
	if (state != 0)
	{
		DEBUG("%s\n", mysql_error(connection));
		ret = -1;
		goto end;//ִ��ʧ�ܣ�����-1  
	}
	result = mysql_store_result(connection);//�õ���ѯ���  
	if (result == (MYSQL_RES *)NULL)
	{
		DEBUG("%s\n", mysql_error(connection));
		ret = -2;
		goto end;//ִ��ʧ�ܣ�����-1  
	}
	ret = result->row_count;
end:if (result != NULL)
{
	mysql_free_result(result);
	result = NULL;
}
	return ret;
}
//���ݿ�ִ�У�������Ӱ��ļ�¼����
int exec_db(const char *SQL, MYSQL *connection)//ִ��û�з������ݼ���SQL���  
{
	int state = mysql_query(connection, SQL);//ִ��SQL���  
	if (state != 0)
	{
		DEBUG("%s\n", mysql_error(connection));
		return -1;
	}
	int count = mysql_affected_rows(connection);
	//	DEBUG("query is ok, %u rows affected\n", count);
	return count;
}
//���ݿ��ѯ�н�����أ�����result
MYSQL_RES * query_db_return_result(const char *SQL, MYSQL *connection)//ִ���з������ݼ���SQL���  
{
	MYSQL_RES *result = NULL;
	int ret = -1;
	int state = mysql_query(connection, SQL);//ִ��SQL���  
	if (state != 0)
	{
		DEBUG("%s\n", mysql_error(connection));
		return NULL;
	}
	result = mysql_store_result(connection);//�õ���ѯ���  
	if (result == (MYSQL_RES *)NULL)
	{
		DEBUG("%s\n", mysql_error(connection));
	}
	return result;
}