#include"service_basic.h"

unsigned int tokenFreshTime = 0;


void getServiceConfig(void)
{
	tokenFreshTime = atoi(getConfig("ThreadpoolQuitType"));
}

void loadCacheNecessary(void)
{
	MysqlBlock * block = mysqlConnectionPool_GetOneConn();
	//����nick cache
	//�����û�token cache,��ʱ���ӣ�������̫����Ҫ���������û���ȡ�������û���token�����ص�cache

	//����notice cache

	//����appVer cache
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
		log_print(L_WARNING , "appVersion insert in cache fail ,this will lead to client not update ������");
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
	//Ӧ�ü�һ������У�飬�Ժ��
	if (strcmp(filter.cmd, "regist") != 0 && strcmp(filter.cmd, "login") != 0)
	{
		//����ʧ��json���������������
		retChar = makeErrorJson(ERR_INPUT_ARGUMENT_WRONG, gerErrMsg(ERR_INPUT_ARGUMENT_WRONG));
		goto end;
	}
	if (strlen(filter.phone) != 11)
	{
		//����ʧ��json���������ֻ��������󡱣��ⲿ�ܿ��ܲ���Ҫ���ֻ���Ҫ��ǰУ�飩
		retChar = makeErrorJson(ERR_PHONE_NUMBER_WRONG, gerErrMsg(ERR_PHONE_NUMBER_WRONG));
		goto end;
	}
	strcpy(filter.account , filter.phone);
	//���жϸ��ֻ����Ƿ�ע���
	ret = UserQueryReturnItemNum(state->fd_sql->conn, &filter);

	if(strcmp(filter.cmd, "regist") == 0 && ret != 0)
	{
		//����ʧ��json���������ֻ������Ѿ�ע�ᡰ
		retChar = makeErrorJson(ERR_PHONE_NUMBER_DUPLICATE, gerErrMsg(ERR_PHONE_NUMBER_DUPLICATE));
		goto end;
	}
	if (strcmp(filter.cmd, "login") == 0 && ret <= 0)
	{
		//����ʧ��json �� �ֻ�����δע��
		retChar = makeErrorJson(ERR_PHONE_NUMBER_NOT_REGIST, gerErrMsg(ERR_PHONE_NUMBER_NOT_REGIST));
		goto end;
	}
	//���cache���Ƿ����Ѿ����ɵ���֤��
	returnMsgCode = cacheQuery(CACHE_COMM , filter.phone);
	if (returnMsgCode != NULL)
	{
		//�����֤���ʱЧ
		if (returnMsgCode->timeStamp + returnMsgCode->freshTime > time(0))
		{
			//�������Ч�����سɹ�json
			retChar = makeErrorJson(0, "ok");
		}
	}

	//��Ҫ���õ�����������֤ƽ̨�������ֻ��ţ�����һ��У����
	returnMsgCode = malloc(sizeof(MsgCode));
	phoneNumber = malloc(12);
	memset(returnMsgCode, 0, sizeof(MsgCode));
	memset(phoneNumber, 0, 12);
	ret = msgGatewayCode(MSG_WANGYI , filter.phone , 6 , 0 , returnMsgCode);
	if (ret == 0)
	{
		strcpy(phoneNumber , filter.phone);
		cacheInsert(CACHE_COMM , phoneNumber , returnMsgCode);
		//���سɹ�json
		retChar = makeErrorJson(0 , "ok");
	}
	else
	{
		//���ش���json������ԭ��Ϊ�������ڲ������޷�������֤��
		free(phoneNumber);
		free(returnMsgCode);
		retChar = makeErrorJson(ret, gerErrMsg(ret));
	}

end:
	//�����ɵ�json�滻��buffer��
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
	//Ӧ�ü�һ������У�飬�Ժ��

	if (strlen(filter.phone) != 11 || strlen(filter.password) != 6)
	{
		//����ʧ��json���������ֻ��������󡱣��ⲿ�ܿ��ܲ���Ҫ���ֻ���Ҫ��ǰУ�飩
		retChar = makeErrorJson(ERR_PHONE_NUMBER_WRONG, gerErrMsg(ERR_PHONE_NUMBER_WRONG));
		goto end;

	}
	//�ɹ��õ��ֻ����Լ���֤��
	//ͨ���ֻ��ŵ��ڴ���ȡ����ǰ��֤�룬�����ڻ�ȡ�����Ƚ�
	code = cacheQuery(CACHE_COMM , filter.phone);
	if (code == NULL)
	{
		//������û�и�У����
		retChar = makeErrorJson(ERR_NOT_FIND_CACHE, gerErrMsg(ERR_NOT_FIND_CACHE));
		goto end;
	}
	if (code->timeStamp + code->freshTime < time(0))
	{
		//��֤�����
		retChar = makeErrorJson(ERR_MSG_CODE_OUT_OF_DATE, gerErrMsg(ERR_MSG_CODE_OUT_OF_DATE));
		goto end;
	}
	if (strcmp(filter.password, code->code) != 0)
	{
		//У����ƥ��ʧ��
		retChar = makeErrorJson(ERR_MSG_CODE_WRONG, gerErrMsg(ERR_MSG_CODE_WRONG));
		goto end;
	}

	user = malloc(sizeof(User));
	memset(user , 0 , sizeof(User));
	//��֤����֤ͨ��,ɾ������
	cacheDelete(CACHE_COMM , filter.phone);
	//����user����������
	strcpy(user->account, filter.phone);
	strcpy(user->phone, filter.phone);
	strcpy(user->password, " ");
	strcpy(user->accType, "p");
	strcpy(user->nickName, "test");
	user->createTime = time(0);
	user->lastModifyTime = user->createTime;
	user->tokenFreshTimeStamp = user->lastModifyTime;
	//����token
	token = malloc(18);
	memset(token, 0, 18);
	srand_get_uuid(uuid);
	HexToStr(token , uuid , 8);
	strcpy(user->token, token);
	//��user���в���һ������
	ret = UserInsert(state->fd_sql->conn, user);
	if (ret != 1)
	{
		free(user);
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}
	//����token �Լ� ����ע����Ϣ
	retChar = UserGenerateJson(user , "id createTime lastModifyTime");
	//token ����cache
	cacheInsert(CACHE_TOKEN, token, user);
	
end:
	//�����ɵ�json�滻��buffer��
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
	//Ӧ�ü�һ������У�飬�Ժ��
	strcpy(account , user.account);
	memset(&user, 0, sizeof(User));
	strcpy(user.account , account);
	ret = UserQueryReturnItemNum(state->fd_sql->conn, &user);
	if (ret != 0)
	{
		//�˺��Ѿ���ע�ᣬ���߳���
		if(ret < 0)
			retChar = makeErrorJson(ret, gerErrMsg(ret));
		else
			retChar = makeErrorJson(ERR_PHONE_NUMBER_DUPLICATE, gerErrMsg(ERR_PHONE_NUMBER_DUPLICATE));
		goto end;
	}
	obj = malloc(sizeof(User));
	memset(obj, 0, sizeof(User));
	UserParseJson(state->buffer, obj);
	//����user����������
	if (!strlen(obj->accType))
		strcpy(obj->accType , "n");
	if (!strlen(obj->nickName))
		strcpy(obj->nickName, "test");
	obj->createTime = time(0);
	obj->lastModifyTime = obj->createTime;
	obj->tokenFreshTimeStamp = obj->lastModifyTime;
	//����token
	token = malloc(18);
	memset(token, 0, 18);
	srand_get_uuid(uuid);
	HexToStr(token, uuid, 8);
	strcpy(obj->token, token);
	//��user���в���һ������
	ret = UserInsert(state->fd_sql->conn, obj);
	if (ret != 1)
	{
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}
	//token ����cache
	cacheInsert(CACHE_TOKEN, token, obj);
	//����token �Լ� ����ע����Ϣ
	retChar = UserGenerateJson(obj , "id createTime lastModifyTime");

end:
	//�����ɵ�json�滻��buffer��
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
	//Ӧ�ü�һ������У�飬�Ժ��

	//���ж��˺��Ƿ�ע���
	ret = UserQueryrReturnResult(state->fd_sql->conn,NULL ,&filter, 0, 0, &user);
	if (ret != 1)
	{
		//�������û��ע��
		if(ret < 0)
			retChar = makeErrorJson(ret, gerErrMsg(ret));
		else
			retChar = makeErrorJson(ERR_ACCOUNT_NOT_REGIST, gerErrMsg(ERR_ACCOUNT_NOT_REGIST));
		goto end;
	}
	//���������token����ɾ����token
	if(strlen(user->token) == 16)
		cacheDelete(CACHE_TOKEN , user->token);

	//������token���滻ԭtoken
	token = malloc(18);
	memset(token, 0, 18);
	srand_get_uuid(uuid);
	HexToStr(token, uuid, 8);
	strcpy(user->token, token);
	user->tokenFreshTimeStamp = time(0);
	//token ����cache
	cacheInsert(CACHE_TOKEN, token, user);
	//�޸�user����token�ֶ�
	memset(&filter, 0, sizeof(User));
	filter.id = user->id;
	strcpy(filter.token , user->token);
	ret = UserModify(state->fd_sql->conn, &filter);
	if (ret != 1)
	{
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}
	//����token �Լ� ����ע����Ϣ
	retChar = UserGenerateJson(user , "id createTime lastModifyTime");

end:
	//�����ɵ�json�滻��buffer��
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
	//�����ɵ�json�滻��buffer��
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
		//cache��û�в鵽�ÿͻ��ˣ�ֱ�Ӹ������˳��ɹ�
		retChar = makeErrorJson(ERR_NOT_LOGIN_YET, gerErrMsg(ERR_NOT_LOGIN_YET));
		goto end;
	}
	//token�Ƿ����У��
	if ((retUser->tokenFreshTimeStamp + tokenFreshTime * 3600) < time(0))
	{
		cacheDelete(CACHE_TOKEN, filter.token);
		retChar = makeErrorJson(ERR_TOKEN_OUT_OF_DATE, gerErrMsg(ERR_TOKEN_OUT_OF_DATE));
		goto end;
	}
	id = retUser->id;
	//��cache��ɾ�� ���û�token
	cacheDelete(CACHE_TOKEN , filter.token);
	memset(&filter, 0, sizeof(User));
	filter.id = id;
	strcpy(filter.token, " ");//�ÿո����ԭtoken���൱��������ݿ��е�token
	ret = UserModify(state->fd_sql->conn , &filter);
	if(ret != 1)
	{
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}
	retChar = makeErrorJson(0, gerErrMsg(0));
end:
	//�����ɵ�json�滻��buffer��
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
		//�û�û�е�¼����
		retChar = makeErrorJson(ERR_NOT_LOGIN_YET, gerErrMsg(ERR_NOT_LOGIN_YET));
		goto end;
	}
	//token�Ƿ����У��
	if ((retUser->tokenFreshTimeStamp + tokenFreshTime * 3600) < time(0))
	{
		cacheDelete(CACHE_TOKEN, filter.token);
		retChar = makeErrorJson(ERR_TOKEN_OUT_OF_DATE, gerErrMsg(ERR_TOKEN_OUT_OF_DATE));
		goto end;
	}
	//����cache�е�token����
	updateObj(retUser, &filter);
	retUser->tokenFreshTimeStamp = time(0);
	retChar = makeErrorJson(0, "ok");
	//��user�����ݽ����޸�
	filter.id = retUser->id;
	ret = UserModify(state->fd_sql->conn, &filter);
	if (ret != 1)
	{
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}

end:
	//�����ɵ�json�滻��buffer��
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
		//ִ��ɾ��������û�û�е�¼
		retChar = makeErrorJson(ERR_NOT_LOGIN_YET, gerErrMsg(ERR_NOT_LOGIN_YET));
		goto end;
	}
	//token�Ƿ����У��
	if ((retUser->tokenFreshTimeStamp + tokenFreshTime * 3600) < time(0))
	{
		cacheDelete(CACHE_TOKEN, filter.token);
		retChar = makeErrorJson(ERR_TOKEN_OUT_OF_DATE, gerErrMsg(ERR_TOKEN_OUT_OF_DATE));
		goto end;
	}
	if (strcmp(retUser->accType, "a") != 0)//a ����Ϊ����Ա������ط�����һ��Ȩ�޹���
	{
		//ִ��ɾ�����û����Ͳ��ǹ���Ա����
		retChar = makeErrorJson(ERR_NO_PERMISSION, gerErrMsg(ERR_NO_PERMISSION));
		goto end;
	}
	memset(filter.token,0 , 20);
	ret = UserQueryrReturnResult(state->fd_sql->conn, NULL ,&filter, 0, 0, &obj);
	if (ret != 1)
	{
		//ɾ�����û�ID�����ڻ��߳���
		if(ret < 0)
			retChar = makeErrorJson(ret, gerErrMsg(ret));
		else
			retChar = makeErrorJson(ERR_ACCOUNT_NOT_REGIST, gerErrMsg(ERR_ACCOUNT_NOT_REGIST));
		goto end;
	}
	//ɾ�����û�token����
	if(strlen(obj->token) == 16)
		cacheDelete(CACHE_TOKEN , obj->token);
	memset(&filter, 0, sizeof(User));
	filter.id = obj->id;
	free(obj);
	//user��ɾ����¼
	ret = UserDelete(state->fd_sql->conn, &filter);
	if (ret != 1)
	{
		retChar = makeErrorJson(ret, gerErrMsg(ret));
		goto end;
	}
	retChar = makeErrorJson(0, "ok");
end:
	//�����ɵ�json�滻��buffer��
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);

}
//֪ͨ����
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
		//�û�û�е�¼
		retChar = makeErrorJson(ERR_NOT_LOGIN_YET, gerErrMsg(ERR_NOT_LOGIN_YET));
		goto end;
	}
	//���ĳ���û�֪ͨ��key
	strcpy(noticeKey, "n_");
	strcat(noticeKey , retUser->account);
	retNotice = cacheQuery(CACHE_COMM, noticeKey);
	if (retNotice == NULL)
	{
		//û��֪ͨ
		retChar = makeErrorJson(ERR_NOT_NOTICE_YET, gerErrMsg(ERR_NOT_NOTICE_YET));
		goto end;
	}
	retChar = NoticeGenerateJson(retNotice , "id createTime lastModifyTime");

end:
	//�����ɵ�json�滻��buffer��
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);
}
//�������
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
	//�����֤ͨ��
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
	//�����ɵ�json�滻��buffer��
	strcpy(state->buffer, retChar);
	state->buffer_used = strlen(retChar);
	free(retChar);
}


