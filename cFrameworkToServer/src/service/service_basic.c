#include"service_basic.h"

unsigned int tokenFreshTime = 0;


void getServiceConfig(void)
{
	tokenFreshTime = atoi(getConfig("ThreadpoolQuitType"));
}

void loadCacheNecessary(void)
{
	MysqlBlock * block = mysqlConnectionPool_GetOneConn();
	//导入nick cache
	//导入用户token cache,暂时不加，工作量太大，需要遍历所有用户表，取出所有用户的token，加载到cache

	//导入notice cache

	//导入appVer cache
	AppVer *ver = NULL;
	char * key = NULL;
	int ret = AppVerQueryrReturnResult(block->conn, "select * from appver where id=(select max(id) from appver);" , NULL , 0 , 0 , &ver);
	if (ret == 1)
	{
		key = malloc(10);
		memset(key, 0, 10);
		strcpy(key, "appVersion");
		cacheInsert(CACHE_COMM, key, ver);
	}
	else
		log_print(L_WARNING , "appVersion insert in cache fail ,this will lead to client not update ！！！");
}

void verifyBefore(void * prm)
{
	User filter;
	int ret = -1;
	fd_state * state = prm;
	char * retChar = NULL;
	MsgCode * returnMsgCode = NULL;
	char * phoneNumber = NULL;
	memset(&filter, 0 , sizeof(User));
	UserParseJson(state->buffer , &filter);
	//应该加一道参数校验，以后加
	if (strcmp(filter.cmd, "regist") != 0 && strcmp(filter.cmd, "login") != 0)
	{
		//返回失败json，“输入参数有误“
		retChar = makeErrorJson(ERR_INPUT_ARGUMENT_WRONG, gerErrMsg(ERR_INPUT_ARGUMENT_WRONG));
		goto end;
	}
	if (strlen(filter.phone) != 11)
	{
		//返回失败json，“输入手机号码有误”（这部很可能不需要，手机需要提前校验）
		retChar = makeErrorJson(ERR_PHONE_NUMBER_WRONG, gerErrMsg(ERR_PHONE_NUMBER_WRONG));
		goto end;
	}
	strcpy(filter.account , filter.phone);
	//先判断该手机号是否注册过
	ret = UserQueryReturnItemNum(state->fd_sql->conn, &filter);

	if(strcmp(filter.cmd, "regist") == 0 && ret != 0)
	{
		//返回失败json，“输入手机号码已经注册“
		retChar = makeErrorJson(ERR_PHONE_NUMBER_DUPLICATE, gerErrMsg(ERR_PHONE_NUMBER_DUPLICATE));
		goto end;
	}
	if (strcmp(filter.cmd, "login") == 0 && ret <= 0)
	{
		//返回失败json ， 手机号码未注册
		retChar = makeErrorJson(ERR_PHONE_NUMBER_NOT_REGIST, gerErrMsg(ERR_PHONE_NUMBER_NOT_REGIST));
		goto end;
	}
	//检查cache中是否有已经生成的验证码
	returnMsgCode = cacheQuery(CACHE_COMM , filter.phone);
	if (returnMsgCode != NULL)
	{
		//检查验证码的时效
		if (returnMsgCode->timeStamp + returnMsgCode->freshTime > time(0))
		{
			//如果还有效，返回成功json
			retChar = makeErrorJson(0, "ok");
		}
	}

	//需要调用第三方短信验证平台，传入手机号，返回一个校验码
	returnMsgCode = malloc(sizeof(MsgCode));
	phoneNumber = malloc(12);
	memset(returnMsgCode, 0, sizeof(MsgCode));
	memset(phoneNumber, 0, 12);
	ret = msgGatewayCode(MSG_WANGYI , filter.phone , 6 , 0 , returnMsgCode);
	if (ret == 0)
	{
		strcpy(phoneNumber , filter.phone);
		cacheInsert(CACHE_COMM , phoneNumber , returnMsgCode);
		//返回成功json
		retChar = makeErrorJson(0 , "ok");
	}
	else
	{
		//返回错误json，错误原因为服务器内部错误，无法发送验证码
		free(phoneNumber);
		free(returnMsgCode);
		retChar = makeErrorJson(ret, gerErrMsg(ret));
	}

end:
	//将生成的json替换到buffer中
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);
}

void verifyAfter(void * prm)
{
	User filter;
	User *user = NULL;
	fd_state * state = prm;
	char * retChar = NULL;
	MsgCode * code = NULL;
	char * token = NULL;
	char uuid[16];
	int ret = 0;
	memset(&filter, 0, sizeof(User));
	UserParseJson(state->buffer, &filter);
	//应该加一道参数校验，以后加

	if (strlen(filter.phone) != 11 || strlen(filter.password) != 6)
	{
		//返回失败json，“输入手机号码有误”（这部很可能不需要，手机需要提前校验）
		retChar = makeErrorJson(ERR_PHONE_NUMBER_WRONG, gerErrMsg(ERR_PHONE_NUMBER_WRONG));
		goto end;

	}
	//成功得到手机号以及验证码
	//通过手机号到内存中取出先前验证码，与现在获取的作比较
	code = cacheQuery(CACHE_COMM , filter.phone);
	if (code == NULL)
	{
		//缓存中没有该校验码
		retChar = makeErrorJson(ERR_NOT_FIND_CACHE, gerErrMsg(ERR_NOT_FIND_CACHE));
		goto end;
	}
	if (code->timeStamp + code->freshTime < time(0))
	{
		//验证码过期
		retChar = makeErrorJson(ERR_MSG_CODE_OUT_OF_DATE, gerErrMsg(ERR_MSG_CODE_OUT_OF_DATE));
		goto end;
	}
	if (strcmp(filter.password, code->code) != 0)
	{
		//校验码匹配失败
		retChar = makeErrorJson(ERR_MSG_CODE_WRONG, gerErrMsg(ERR_MSG_CODE_WRONG));
		goto end;
	}

	user = malloc(sizeof(User));
	memset(user , 0 , sizeof(User));
	//验证码验证通过,删除缓存
	cacheDelete(CACHE_COMM , filter.phone);
	//补足user其它必填项
	strcpy(user->account, filter.phone);
	strcpy(user->phone, filter.phone);
	strcpy(user->password, " ");
	strcpy(user->accType, "p");
	strcpy(user->nickName, "test");
	user->createTime = time(0);
	user->lastModifyTime = user->createTime;
	user->tokenFreshTimeStamp = user->lastModifyTime;
	//生成token
	token = malloc(18);
	memset(token, 0, 18);
	srand_get_uuid(uuid);
	HexToStr(token , uuid , 8);
	strcpy(user->token, token);
	//向user表中插入一条数据
	ret = UserInsert(state->fd_sql->conn, user);
	if (ret != 1)
	{
		free(user);
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}
	//返回token 以及 其它注册信息
	retChar = UserGenerateJson(user , "id createTime lastModifyTime");
	//token 插入cache
	cacheInsert(CACHE_TOKEN, token, user);
	
end:
	//将生成的json替换到buffer中
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);
}

void userRegister(void * prm)
{
	User user;
	User * obj = NULL;
	fd_state * state = prm;
	char * retChar = NULL;
	char * token = NULL;
	char uuid[16];
	int ret = 0;
	char account[20];
	memset(account, 0, 20);
	memset(&user, 0, sizeof(User));
	UserParseJson(state->buffer, &user);
	//应该加一道参数校验，以后加
	strcpy(account , user.account);
	memset(&user, 0, sizeof(User));
	strcpy(user.account , account);
	ret = UserQueryReturnItemNum(state->fd_sql->conn, &user);
	if (ret != 0)
	{
		//账号已经被注册，或者出错
		if(ret < 0)
			retChar = makeErrorJson(ret, gerErrMsg(ret));
		else
			retChar = makeErrorJson(ERR_PHONE_NUMBER_DUPLICATE, gerErrMsg(ERR_PHONE_NUMBER_DUPLICATE));
		goto end;
	}
	obj = malloc(sizeof(User));
	memset(obj, 0, sizeof(User));
	UserParseJson(state->buffer, obj);
	//补足user其它必填项
	if (!strlen(obj->accType))
		strcpy(obj->accType , "n");
	if (!strlen(obj->nickName))
		strcpy(obj->nickName, "test");
	obj->createTime = time(0);
	obj->lastModifyTime = obj->createTime;
	obj->tokenFreshTimeStamp = obj->lastModifyTime;
	//生成token
	token = malloc(18);
	memset(token, 0, 18);
	srand_get_uuid(uuid);
	HexToStr(token, uuid, 8);
	strcpy(obj->token, token);
	//向user表中插入一条数据
	ret = UserInsert(state->fd_sql->conn, obj);
	if (ret != 1)
	{
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}
	//token 插入cache
	cacheInsert(CACHE_TOKEN, token, obj);
	//返回token 以及 其它注册信息
	retChar = UserGenerateJson(obj , "id createTime lastModifyTime");

end:
	//将生成的json替换到buffer中
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);

}

void userLogin(void * prm)
{
	User *user = NULL;
	fd_state * state = prm;
	char * retChar = NULL;
	char * token = NULL;
	char uuid[16];
	int ret = 0;
	User filter;
	int id = 0;
	memset(&filter, 0, sizeof(User));
	UserParseJson(state->buffer, &filter);
	//应该加一道参数校验，以后加

	//先判断账号是否注册过
	ret = UserQueryrReturnResult(state->fd_sql->conn,NULL ,&filter, 0, 0, &user);
	if (ret != 1)
	{
		//出错或者没有注册
		if(ret < 0)
			retChar = makeErrorJson(ret, gerErrMsg(ret));
		else
			retChar = makeErrorJson(ERR_ACCOUNT_NOT_REGIST, gerErrMsg(ERR_ACCOUNT_NOT_REGIST));
		goto end;
	}
	//如果存在老token，先删除老token
	if(strlen(user->token) == 16)
		cacheDelete(CACHE_TOKEN , user->token);

	//生成新token，替换原token
	token = malloc(18);
	memset(token, 0, 18);
	srand_get_uuid(uuid);
	HexToStr(token, uuid, 8);
	strcpy(user->token, token);
	user->tokenFreshTimeStamp = time(0);
	//token 插入cache
	cacheInsert(CACHE_TOKEN, token, user);
	//修改user表中token字段
	memset(&filter, 0, sizeof(User));
	filter.id = user->id;
	strcpy(filter.token , user->token);
	ret = UserModify(state->fd_sql->conn, &filter);
	if (ret != 1)
	{
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}
	//返回token 以及 其它注册信息
	retChar = UserGenerateJson(user , "id createTime lastModifyTime");

end:
	//将生成的json替换到buffer中
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);
}

void userStatus(void * prm)
{
	User filter;
	User * retUser = NULL;
	char * retChar = NULL;
	fd_state * state = prm;
	memset(&filter, 0, sizeof(User));
	UserParseJson(state->buffer, &filter);
	retUser = cacheQuery(CACHE_TOKEN , filter.token);
	if (retUser == NULL)
	{
		retChar = makeErrorJson(ERR_NOT_LOGIN_YET, gerErrMsg(ERR_NOT_LOGIN_YET));
		goto end;
	}
	if ((retUser->tokenFreshTimeStamp + tokenFreshTime * 3600) < time(0))
	{
		cacheDelete(CACHE_TOKEN, filter.token);
		retChar = makeErrorJson(ERR_TOKEN_OUT_OF_DATE, gerErrMsg(ERR_TOKEN_OUT_OF_DATE));
		goto end;
	}
	retChar = makeErrorJson(0, "ok");
end:
	//将生成的json替换到buffer中
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);
}

void userLogout(void * prm)
{
	User filter;
	User * retUser = NULL;
	char * retChar = NULL;
	fd_state * state = prm;
	int id = 0, ret = 0;;
	memset(&filter, 0, sizeof(User));
	UserParseJson(state->buffer, &filter);
	retUser = cacheQuery(CACHE_TOKEN, filter.token);
	if (retUser == NULL)
	{
		//cache中没有查到该客户端，直接告诉它退出成功
		retChar = makeErrorJson(ERR_NOT_LOGIN_YET, gerErrMsg(ERR_NOT_LOGIN_YET));
		goto end;
	}
	//token是否过期校验
	if ((retUser->tokenFreshTimeStamp + tokenFreshTime * 3600) < time(0))
	{
		cacheDelete(CACHE_TOKEN, filter.token);
		retChar = makeErrorJson(ERR_TOKEN_OUT_OF_DATE, gerErrMsg(ERR_TOKEN_OUT_OF_DATE));
		goto end;
	}
	id = retUser->id;
	//从cache中删除 该用户token
	cacheDelete(CACHE_TOKEN , filter.token);
	memset(&filter, 0, sizeof(User));
	filter.id = id;
	strcpy(filter.token, " ");//用空格替代原token，相当于清除数据库中的token
	ret = UserModify(state->fd_sql->conn , &filter);
	if(ret != 1)
	{
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}
	retChar = makeErrorJson(0, gerErrMsg(0));
end:
	//将生成的json替换到buffer中
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);
}

void userModify(void * prm)
{
	User filter;
	User * retUser = NULL;
	char * retChar = NULL;
	int ret = 0;
	fd_state * state = prm;
	memset(&filter, 0, sizeof(User));
	UserParseJson(state->buffer, &filter);
	retUser = cacheQuery(CACHE_TOKEN, filter.token);
	if (retUser == NULL)
	{
		//用户没有登录错误
		retChar = makeErrorJson(ERR_NOT_LOGIN_YET, gerErrMsg(ERR_NOT_LOGIN_YET));
		goto end;
	}
	//token是否过期校验
	if ((retUser->tokenFreshTimeStamp + tokenFreshTime * 3600) < time(0))
	{
		cacheDelete(CACHE_TOKEN, filter.token);
		retChar = makeErrorJson(ERR_TOKEN_OUT_OF_DATE, gerErrMsg(ERR_TOKEN_OUT_OF_DATE));
		goto end;
	}
	//更新cache中的token数据
	updateObj(retUser, &filter);
	retUser->tokenFreshTimeStamp = time(0);
	retChar = makeErrorJson(0, "ok");
	//对user表数据进行修改
	filter.id = retUser->id;
	ret = UserModify(state->fd_sql->conn, &filter);
	if (ret != 1)
	{
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}

end:
	//将生成的json替换到buffer中
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);

}

void userDelete(void * prm)
{
	User filter;
	User * obj = NULL;
	User * retUser = NULL;
	char * retChar = NULL;
	int ret = 0;
	fd_state * state = prm;
	memset(&filter, 0, sizeof(User));
	UserParseJson(state->buffer, &filter);
	retUser = cacheQuery(CACHE_TOKEN, filter.token);
	if (retUser == NULL)
	{
		//执行删除命令的用户没有登录
		retChar = makeErrorJson(ERR_NOT_LOGIN_YET, gerErrMsg(ERR_NOT_LOGIN_YET));
		goto end;
	}
	//token是否过期校验
	if ((retUser->tokenFreshTimeStamp + tokenFreshTime * 3600) < time(0))
	{
		cacheDelete(CACHE_TOKEN, filter.token);
		retChar = makeErrorJson(ERR_TOKEN_OUT_OF_DATE, gerErrMsg(ERR_TOKEN_OUT_OF_DATE));
		goto end;
	}
	if (strcmp(retUser->accType, "a") != 0)//a 臆想为管理员，这个地方得想一下权限管理
	{
		//执行删除的用户类型不是管理员类型
		retChar = makeErrorJson(ERR_NO_PERMISSION, gerErrMsg(ERR_NO_PERMISSION));
		goto end;
	}
	memset(filter.token,0 , 20);
	ret = UserQueryrReturnResult(state->fd_sql->conn, NULL ,&filter, 0, 0, &obj);
	if (ret != 1)
	{
		//删除的用户ID不存在或者出错
		if(ret < 0)
			retChar = makeErrorJson(ret, gerErrMsg(ret));
		else
			retChar = makeErrorJson(ERR_ACCOUNT_NOT_REGIST, gerErrMsg(ERR_ACCOUNT_NOT_REGIST));
		goto end;
	}
	//删除该用户token缓存
	if(strlen(obj->token) == 16)
		cacheDelete(CACHE_TOKEN , obj->token);
	memset(&filter, 0, sizeof(User));
	filter.id = obj->id;
	free(obj);
	//user表删除记录
	ret = UserDelete(state->fd_sql->conn, &filter);
	if (ret != 1)
	{
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}
	retChar = makeErrorJson(0, "ok");
end:
	//将生成的json替换到buffer中
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);

}
//通知推送
void noticeGet(void * prm)
{
	Notice filter;
	User * retUser = NULL;
	Notice * retNotice = NULL;
	char * retChar = NULL;
	char noticeKey[30];
	memset(noticeKey, 0, 30);
	fd_state * state = prm;
	memset(&filter, 0, sizeof(filter));
	NoticeParseJson(state->buffer, &filter);
	retUser = cacheQuery(CACHE_TOKEN, filter.token);
	if (retUser == NULL)
	{
		//用户没有登录
		retChar = makeErrorJson(ERR_NOT_LOGIN_YET, gerErrMsg(ERR_NOT_LOGIN_YET));
		goto end;
	}
	//组合某个用户通知的key
	strcpy(noticeKey, "n_");
	strcat(noticeKey , retUser->account);
	retNotice = cacheQuery(CACHE_COMM, noticeKey);
	if (retNotice == NULL)
	{
		//没有通知
		retChar = makeErrorJson(ERR_NOT_NOTICE_YET, gerErrMsg(ERR_NOT_NOTICE_YET));
		goto end;
	}
	retChar = NoticeGenerateJson(retNotice , "id createTime lastModifyTime");

end:
	//将生成的json替换到buffer中
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);
}
//软件更新
void appVer(void * prm)
{
	AppVer filter;
	User * retUser = NULL;
	AppVer * retAppVer = NULL;
	char * retChar = NULL;
	fd_state * state = prm;
	memset(&filter, 0, sizeof(AppVer));
	AppVerParseJson(state->buffer, &filter);
	retUser = cacheQuery(CACHE_TOKEN, filter.token);
	if (retUser == NULL)
	{
		retChar = makeErrorJson(ERR_NOT_LOGIN_YET, gerErrMsg(ERR_NOT_LOGIN_YET));
		goto end;
	}
	//身份验证通过
	retAppVer = cacheQuery(CACHE_COMM , "appVersion");
	if (retAppVer == NULL)
	{
		retChar = makeErrorJson(ERR_APP_VERSION_NOT_FOUND_IN_CACHE, gerErrMsg(ERR_APP_VERSION_NOT_FOUND_IN_CACHE));
		goto end;
	}
	if (strcmp(filter.name, retAppVer->name) != 0)
		retChar = AppVerGenerateJson(retAppVer , "id");
	else
		retChar = makeErrorJson(ERR_APPVERSION_NOT_UPDATE, gerErrMsg(ERR_APPVERSION_NOT_UPDATE));
	
end:
	//将生成的json替换到buffer中
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);
}


