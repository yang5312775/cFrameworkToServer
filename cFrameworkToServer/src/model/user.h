#ifndef _USER_H_
#define _USER_H_
#include"model.h"

typedef struct {
	int id;            //自增用户ID    自增
	char account[20];  //用户账号      必填
	char password[20]; //用户密码      必填
	char token[20];    //临时口令
	char accType[2];      //账号类别   必填  a为管理员 p为手机注册 n为账号注册
	char phone[16];    //用户电话
	char mail[32];     //用户邮箱
	char realName[20]; //真名
	char nickName[20]; //昵称          必填
	char prcId[20];   //身份证号
	char sex[2];          //性别
	int	 birthday;     //生日
	char country[20];  //国家
	char province[20]; //省份
	char city[20];     //城市
	char district[20]; //区
	char address[256]; //详细地址
	char headPortrait[256]; //头像地址
	char sign[256];      //个性签名
	int  createTime;   //注册时间         必填
	int  lastModifyTime; //最后修改时间   必填
	//以下为数据库之外参数，主要是配合json解析、生成工作
	int tokenFreshTimeStamp;//token活跃时间戳
	char cmd[10];      //发起注册，还是发起登录
}User;


/**************************数据库操作接口**************************************/
//插入一条数据，必填字段必须有值，否则插入失败
int UserInsert(MYSQL * connection, User * obj);
//查询数据条数，哪个字段有值，哪个字段就会成为条件之一
int UserQueryReturnItemNum(MYSQL * connection, User * filter);
//查询多条数据，filter哪个字段有值，哪个字段就作为条件，最终结果通过obj返回，数量由返回值返回
int UserQueryrReturnResult(MYSQL * connection, char * sqlManual, User * filter, int limitFrom, int limitCount, User ** obj);
//修改一条数据，哪个字段有值，就修改哪个字段，修改条件为主键id
int UserModify(MYSQL * connection, User * filter);
//删除一条数据，哪个字段有值，哪个字段就会成为条件之一
int UserDelete(MYSQL * connection, User * filter);
/**************************数据库操作接口**************************************/



/**********************结构体数据与json数据互转接口*******************************/
//通过结构体生成json数据
char * UserGenerateJson(User * obj, char * removeField);
//将json解析为结构体数据
int UserparseJson(char * jsonString, User * obj);
/**********************结构体数据与json数据互转接口*******************************/

//更新某个对象的属性
void updateObj(User * des, User * src);

#endif