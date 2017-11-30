#include<model.h>
#include<socket_server.h>
//
void test_table_appUpdate(void) {

	AppVer appUpdate;
	MysqlBlock * block =  mysqlConnectionPool_GetOneConn();
	memset(&appUpdate , 0 , sizeof(AppVer));
	strcpy(appUpdate.type, "a");
	strcpy(appUpdate.name, "icecream");
	strcpy(appUpdate.version, "Ver0.0.1");
	strcpy(appUpdate.level, "1");
	strcpy(appUpdate.path, "http://XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
	appUpdate.createTime = time(0);
	appUpdate.lastModifyTime = appUpdate.createTime;
	int ret = AppVerInsert(block->conn , &appUpdate);

	memset(&appUpdate, 0, sizeof(AppVer));
	strcpy(appUpdate.name, "icecream");
	ret = AppVerQueryReturnItemNum(block->conn, &appUpdate);
	int id = appUpdate.id;
	memset(&appUpdate, 0, sizeof(AppVer));
	appUpdate.id = id;
	strcpy(appUpdate.level, "2");
	ret = AppVerModify(block->conn , &appUpdate);
	memset(&appUpdate, 0, sizeof(AppVer));
	AppVer * returnVal = NULL;
	ret = AppVerQueryrReturnResult(block->conn, NULL, &appUpdate , 0 , 10 , &returnVal);

	char * jsonString = AppVerGenerateJson(returnVal , NULL);
	printf("%s\n" , jsonString);
	AppVerParseJson(jsonString, &appUpdate);
	memset(&appUpdate, 0, sizeof(AppVer));
	returnVal++;
	appUpdate.id = id;
	ret = AppVerDelete(block->conn, &appUpdate);
	mysqlConnectionPool_FreeOneConn(block);

}
