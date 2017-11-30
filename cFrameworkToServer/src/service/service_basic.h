#ifndef _SERVICE_BASIC_H_
#define _SERVICE_BASIC_H_
#include<socket_server.h>
#include<model.h>
#include"cache.h"
#include"message_gateway.h"
//获取服务部分配置参数
void getServiceConfig(void);
//短信验证注册或者登陆第一步
void verifyBefore(void * prm);
//短信验证第二步
void verifyAfter(void * prm);
//用户在线注册，用账号密码注册
void userRegister(void * prm);
//用户登陆，需要账号密码
void userLogin(void * prm);
//查询用户登陆状态
void userStatus(void * prm);
//用户退出
void userLogout(void * prm);
//用户信息修改，包括密码
void userModify(void * prm);
//用户删除，准备给管理员使用，普通用户没有删除用户权限
void userDelete(void * prm);
//获取发送给用户的通知
void noticeGet(void * prm);
//获取最新软件版本
void appVer(void * prm);

#endif
