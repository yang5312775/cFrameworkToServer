#ifndef _NOTICE_H_
#define _NOTICE_H_
#include"model.h"

typedef struct {
	int id;            //自增用户ID    自增
	char title[128]; //主题              必填
	char type[2];  //通知类型            必填    p代表公共通知   v代表定制通知
	int startTime; //开始时间            必填
	int endTime;   //结束时间            必填
	char country[20];  //国家            必填
	char province[20]; //省份 
	char city[20];     //城市
	char district[20]; //区
	char info[1024]; //内容           必填
	int  createTime;   //注册时间         必填
	int  lastModifyTime; //最后修改时间   必填
	//以下为数据库之外参数，主要是配合json解析、生成工作
	char token[20];    //临时口令
}Notice;


/**************************数据库操作接口**************************************/
//插入一条数据，必填字段必须有值，否则插入失败
int NoticeInsert(MYSQL * connection, Notice * obj);
//查询数据条数，哪个字段有值，哪个字段就会成为条件之一
int NoticeQueryReturnItemNum(MYSQL * connection, Notice * filter);
//查询多条数据，filter哪个字段有值，哪个字段就作为条件，最终结果通过obj返回，数量由返回值返回
int NoticeQueryrReturnResult(MYSQL * connection, char * sqlManual, Notice * filter, int limitFrom, int limitCount, Notice ** obj);
//修改一条数据，哪个字段有值，就修改哪个字段，修改条件为主键id
int NoticeModify(MYSQL * connection, Notice * obj);
//删除一条数据，哪个字段有值，哪个字段就会成为条件之一
int NoticeDelete(MYSQL * connection, Notice * obj);
/**************************数据库操作接口**************************************/



/**********************结构体数据与json数据互转接口*******************************/
//通过结构体生成json数据
char * NoticeGenerateJson(Notice * obj, char * removeField);
//将json解析为结构体数据
int NoticeParseJson(char * jsonString, Notice * obj);
/**********************结构体数据与json数据互转接口*******************************/
#endif