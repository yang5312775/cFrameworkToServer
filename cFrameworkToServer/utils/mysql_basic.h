#ifndef _H_MYSQL_BASIC
#define _H_MYSQL_BASIC
#include<basic.h>

//���ݿ��ѯ�޽�����أ�ֻ���ز�ѯ��������
int query_db(const char *SQL, MYSQL *connection);//ִ���з������ݼ���SQL���  
												 //���ݿ�ִ�У�������Ӱ��ļ�¼����
int exec_db(const char *SQL, MYSQL *connection);//ִ��û�з������ݼ���SQL���  
												//���ݿ��ѯ�н�����أ�����result
MYSQL_RES * query_db_return_result(const char *SQL, MYSQL *connection);//ִ���з������ݼ���SQL���  

#endif
