#include<user.h>
#include<socket_server.h>
//
void test_table_user(void) {

	User user;
	MysqlBlock * block =  mysqlConnectionPool_GetOneConn();
	memset(&user , 0 , sizeof(User));
	strcpy(user.account, "yangyong");
	strcpy(user.password, "123456");
	strcpy(user.accType, "n");
	strcpy(user.nickName, "defaultname");
	user.createTime = time(0);
	user.lastModifyTime = user.createTime;
	int ret = UserInsert(block->conn , &user);

	memset(&user, 0, sizeof(User));
	strcpy(user.account, "yangyong");
	strcpy(user.password, "123456");
	ret = UserQueryReturnItemNum(block->conn, &user);
	int id = user.id;
	memset(&user, 0, sizeof(User));
	user.id = id;
	strcpy(user.address, "bei jing shi hai dian qu hou chang cun lu 59 hao!!!");
	user.birthday = 1200000;
	ret = UserModify(block->conn , &user);
	memset(&user, 0, sizeof(User));
	User * returnVal = NULL;
	ret = UserQueryrReturnResult(block->conn,NULL ,  &user , 0 , 10 , &returnVal);

	char * jsonString = UserGenerateJson(returnVal , NULL);
	printf("%s\n" , jsonString);
	UserParseJson(jsonString, &user);
	memset(&user, 0, sizeof(User));
	returnVal++;
	user.id = id;
	ret = UserDelete(block->conn, &user);
	mysqlConnectionPool_FreeOneConn(block);

}
