#ifndef _H_MYSQL_BASIC
#define _H_MYSQL_BASIC
#include<basic.h>
											
int exec_db(const char *SQL, MYSQL *connection);//ִ��û�з������ݼ���SQL���  
												
MYSQL_RES * query_db(const char *SQL, MYSQL *connection);//ִ���з������ݼ���SQL���  

#endif
