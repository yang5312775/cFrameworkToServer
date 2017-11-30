#include"mysql_basic.h"
#include"log.h"
//���ݿ�ִ�У�������Ӱ��ļ�¼����
int exec_db(const char *SQL, MYSQL *connection)//ִ��û�з������ݼ���SQL���  
{
	int state = mysql_query(connection, SQL);//ִ��SQL���  
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
//���ݿ��ѯ�н�����أ�����result
MYSQL_RES * query_db(const char *SQL, MYSQL *connection)//ִ���з������ݼ���SQL���  
{
	MYSQL_RES *result = NULL;
	int ret = -1;
	int state = mysql_query(connection, SQL);//ִ��SQL���  
	if (state != 0)
	{
		log_print(L_ERROR ,"%s\n", mysql_error(connection));
		printf("%s\n", mysql_error(connection));
		return NULL;
	}
	result = mysql_store_result(connection);//�õ���ѯ���  
	if (result == (MYSQL_RES *)NULL)
	{
		log_print(L_ERROR ,"%s\n", mysql_error(connection));
		printf("%s\n", mysql_error(connection));
	}
	return result;
}