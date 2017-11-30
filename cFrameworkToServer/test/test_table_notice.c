#include<notice.h>
#include<socket_server.h>
//
void test_table_notice(void) {

	Notice notice;
	MysqlBlock * block =  mysqlConnectionPool_GetOneConn();
	memset(&notice , 0 , sizeof(Notice));
	strcpy(notice.title, "tongzhi");
	strcpy(notice.type, "n");
	notice.startTime = time(0) - 100000;
	notice.endTime = time(0) + 100000;
	strcpy(notice.info, "zhe shi yi tiao tong zhi!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	notice.createTime = time(0);
	notice.lastModifyTime = notice.createTime;
	int ret = NoticeInsert(block->conn , &notice);

	memset(&notice, 0, sizeof(Notice));
	strcpy(notice.title, "tongzhi");
	ret = NoticeQueryReturnItemNum(block->conn, &notice);
	int id = notice.id;
	memset(&notice, 0, sizeof(Notice));
	notice.id = id;
	strcpy(notice.info, "zhe shi yi tiao tong zhi!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! @@@@@@@@@@@@@@@@@@@@@@@@@");
	ret = NoticeModify(block->conn , &notice);
	memset(&notice, 0, sizeof(Notice));
	Notice * returnVal = NULL;
	ret = NoticeQueryrReturnResult(block->conn, NULL,&notice , 0 , 10 , &returnVal);

	char * jsonString = NoticeGenerateJson(returnVal , NULL);
	printf("%s\n" , jsonString);
	NoticeParseJson(jsonString, &notice);
	memset(&notice, 0, sizeof(Notice));
	returnVal++;
	notice.id = id;
	ret = NoticeDelete(block->conn, &notice);
	mysqlConnectionPool_FreeOneConn(block);

}
