#ifndef _APP_UPDATE_H_
#define _APP_UPDATE_H_
#include"model.h"

typedef struct {
	int id;            //自增用户ID    自增
	char type[2]; //类型              必填
	char name[20];  //软件代号            必填
	char version[20];  //软件版本           必填
	char level[2]; //重要程度              必填
	char path[256];     //软件所在位置     必填
	int  releaseTime;   //发布时间         必填
	int  createTime;   //创建时间         必填
	int  lastModifyTime; //最后修改时间   必填
	//以下为数据库之外参数，主要是配合json解析、生成工作
	char token[20];    //临时口令
}AppVer;


/**************************数据库操作接口**************************************/
//插入一条数据，必填字段必须有值，否则插入失败
int AppVerInsert(MYSQL * connection, AppVer * obj);
//查询一条数据，哪个字段有值，哪个字段就会成为条件之一
int AppVerQueryReturnItemNum(MYSQL * connection, AppVer * filter);
//查询多条数据，filter哪个字段有值，哪个字段就作为条件，最终结果通过obj返回，数量由返回值返回
int AppVerQueryrReturnResult(MYSQL * connection, char * sqlManual, AppVer * filter, int limitFrom, int limitCount, AppVer ** obj);
//修改一条数据，哪个字段有值，就修改哪个字段，修改条件为主键id
int AppVerModify(MYSQL * connection, AppVer * obj);
//删除一条数据，哪个字段有值，哪个字段就会成为条件之一
int AppVerDelete(MYSQL * connection, AppVer * obj);
/**************************数据库操作接口**************************************/



/**********************结构体数据与json数据互转接口*******************************/
//通过结构体生成json数据
char * AppVerGenerateJson(AppVer * obj  , char * removeField);
//将json解析为结构体数据
int AppVerParseJson(char * jsonString, AppVer * obj);
/**********************结构体数据与json数据互转接口*******************************/
#endif