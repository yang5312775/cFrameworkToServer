#include<mysql_basic.h>
#include"user.h"

static void getWhereOrSetString(char * buffer, User * obj, char wos);

int UserInsert(MYSQL * connection , User * obj)
{
	char sql[SQL_LENGTH];
	int ret = -1;
	MYSQL_RES *result = NULL;
	MYSQL_ROW sqlRow;
	 //查询该账号是否可用
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "select * from user where account='%s';", obj->account);
	result = query_db(sql, connection);
	if (result == NULL)
	{
		ret = ERR_MYSQL_QUERY_FAIL;
		goto end;
	}
	if (result->row_count >= 1)
	{
		ret = ERR_USER_REGIST_DUPLICATE;
		goto end;
	}
	mysql_free_result(result);
	result = NULL;
	//插入该条记录
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "insert into user(account,password,token,accType,phone,mail,realName,nickName,prcId,sex,\
				birthday,country,province,city,district,address,headPortrait,sign,createTime,lastModifyTime)\
				value('%s','%s','%s','%s','%s','%s','%s','%s','%s','%s','%d','%s','%s','%s','%s','%s','%s','%s','%d','%d');" \
		, obj->account, obj->password, obj->token, obj->accType, obj->phone, obj->mail, obj->realName, obj->nickName, obj->prcId, obj->sex, \
		obj->birthday, obj->country, obj->province, obj->city, obj->district, obj->address,obj->headPortrait ,obj->sign ,obj->createTime, obj->lastModifyTime);
	printf("%s" , sql);
	ret = exec_db(sql, connection);
	if (ret <= 0)
	{
		ret = ERR_USER_REGIST_INSERT_FAIL;
		goto end;
	}
	//获取插入记录自增ID
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "select id from user where account='%s';", obj->account);
	result = query_db(sql, connection);
	if (result == NULL)
	{
		ret = ERR_MYSQL_QUERY_FAIL;
		goto end;
	}
	if (result->row_count == 0)
	{
		ret = ERR_USER_ERGIST_QUERY_FAIL;
		goto end;
	}
	sqlRow = mysql_fetch_row(result);
	obj->id = atoi(sqlRow[0]);
end:
	if(result != NULL)
		mysql_free_result(result); 
	result = NULL;
	return ret;
}

int UserQueryReturnItemNum(MYSQL * connection, User * filter)
{
	char sql[SQL_LENGTH];
	char arg[SQL_LENGTH];
	int ret = 0;
	MYSQL_RES *result = NULL;
	MYSQL_ROW sqlRow;
	getWhereOrSetString(arg , filter, AND);
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "select count(*) from user %s;", arg);
	result = query_db(sql, connection);
	if (result == NULL)
	{
		ret = ERR_MYSQL_QUERY_FAIL;
		goto end;
	}
	sqlRow = mysql_fetch_row(result);
	ret = atoi(sqlRow[0]);	
end:
	if (result != NULL)
		mysql_free_result(result);
	result = NULL;
	return ret;
}

int UserQueryrReturnResult(MYSQL * connection, char * sqlManual , User * filter, int limitFrom, int limitCount, User ** obj)
{
	char sql[SQL_LENGTH];
	char arg[SQL_LENGTH];
	int ret = -1;
	MYSQL_RES *result = NULL;
	MYSQL_ROW sqlRow;
	if (sqlManual == NULL)
	{
		getWhereOrSetString(arg, filter, AND);
		memset(sql, 0, SQL_LENGTH);
		if (limitCount)
			sprintf(sql, "select * from user %s limit %d,%d;", arg, limitFrom, limitCount);
		else
			sprintf(sql, "select * from user %s ;", arg);
	}
	else
		strcpy(sql, sqlManual);
	result = query_db(sql, connection);
	if (result == NULL)
	{
		ret = ERR_MYSQL_QUERY_FAIL;
		goto end;
	}
	if (result->row_count == 0)
	{
		ret = 0;
		goto end;
	}
	*obj = malloc(sizeof(User) * result->row_count);
	memset(*obj , 0 , sizeof(User) * result->row_count);
	User * p = *obj;
	for (int i = 0; i < result->row_count; i++)
	{
		sqlRow = mysql_fetch_row(result);
		p->id = atoi(sqlRow[0]);
		strcpy(p->account, sqlRow[1]);
		strcpy(p->password, sqlRow[2]);
		strcpy(p->token, sqlRow[3]);
		strcpy(p->accType, sqlRow[4]);
		strcpy(p->phone, sqlRow[5]);
		strcpy(p->mail, sqlRow[6]);
		strcpy(p->realName, sqlRow[7]);
		strcpy(p->nickName, sqlRow[8]);
		strcpy(p->prcId, sqlRow[9]);
		strcpy(p->sex, sqlRow[10]);
		p->birthday = atoi(sqlRow[11]);
		strcpy(p->country, sqlRow[12]);
		strcpy(p->province, sqlRow[13]);
		strcpy(p->city, sqlRow[14]);
		strcpy(p->district, sqlRow[15]);
		strcpy(p->address, sqlRow[16]);
		strcpy(p->headPortrait, sqlRow[17]);
		strcpy(p->sign, sqlRow[18]);
		p->createTime = atoi(sqlRow[19]);
		p->lastModifyTime = atoi(sqlRow[20]);
		p++;
	}
	ret = result->row_count;
end:
	if (result != NULL)
		mysql_free_result(result);
	result = NULL;
	return ret;
}

int UserModify(MYSQL * connection, User * filter)
{
	char sql[SQL_LENGTH];
	char arg[SQL_LENGTH];
	int ret = -1;
	getWhereOrSetString(arg, filter, COMMA);
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "update user %s where id='%d';", arg , filter->id);
	ret = exec_db(sql, connection);
	if (ret < 0)
		return ERR_USER_MODIFY_FAIL;
	return ret;
}

int UserDelete(MYSQL * connection, User * filter)
{
	char sql[SQL_LENGTH];
	char arg[SQL_LENGTH];
	int ret = -1;
	getWhereOrSetString(arg, filter, AND);
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "delete from user %s;", arg);
	ret = exec_db(sql, connection);
	if (ret < 0)
		return ERR_USER_DELETE_FAIL;
	return ret;
}

static void getWhereOrSetString(char * buffer, User * obj , char wos)
{
	char flagString[10];
	memset(buffer, 0, SQL_LENGTH);
	char arg[SQL_LENGTH];
	if (wos == AND)
	{
		strcpy(flagString, " and ");
		strcat(buffer, "where ");
	}	
	else
	{
		strcpy(flagString, " , ");
		strcat(buffer, "set ");
	}	
	char middleFlag = 0;
	if (wos == AND)
	{
		if (obj->id != 0)
		{
			memset(arg, 0, SQL_LENGTH);
			sprintf(arg, "id='%d'", obj->id);
			strcat(buffer, arg);
			middleFlag = 1;
		}
		if (strlen(obj->account) != 0)
		{
			if (middleFlag == 1)
				strcat(buffer, flagString);
			memset(arg, 0, SQL_LENGTH);
			sprintf(arg, "account='%s'", obj->account);
			strcat(buffer, arg);
			middleFlag = 1;
		}
	}
	if (strlen(obj->password) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "password='%s'", obj->password);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->token) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "token='%s'", obj->token);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->accType) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "accType='%s'", obj->accType);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->phone) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "phone='%s'", obj->phone);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->mail) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "mail='%s'", obj->mail);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->realName) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "realName='%s'", obj->realName);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->nickName) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "nickName='%s'", obj->nickName);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->prcId) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "prcId='%s'", obj->prcId);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->sex) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "sex='%s'", obj->sex);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (obj->birthday != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "birthday='%d'", obj->birthday);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->country) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "country='%s'", obj->country);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->province) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "province='%s'", obj->province);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->city) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "city='%s'", obj->city);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->district) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "district='%s'", obj->district);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->address) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "address='%s'", obj->address);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->headPortrait) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "headPortrait='%s'", obj->headPortrait);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->sign) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "sign='%s'", obj->sign);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (obj->createTime != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "createTime='%d'", obj->createTime);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (obj->lastModifyTime != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "lastModifyTime='%d'", obj->lastModifyTime);
		strcat(buffer, arg);
	}
	if (strcmp(buffer, "where ") == 0 || strcmp(buffer, "set ") == 0)
	{
		memset(buffer, 0, SQL_LENGTH);
		strcat(buffer , " ");
	}
}

int UserParseJson(char * jsonString, User * obj)
{
	cJSON *json = NULL;
	cJSON *temp = NULL;
	int ret = 0;
	json = cJSON_Parse(jsonString);
	if (json == NULL)
		return ERR_JSON_PARSE_FAIL;
	temp = cJSON_GetObjectItem(json, "id");
	if (temp != NULL)
		obj->id = temp->valueint;
	temp = cJSON_GetObjectItem(json, "account");
	if (temp != NULL)
		strcpy(obj->account, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "password");
	if (temp != NULL)
		strcpy(obj->password, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "accType");
	if (temp != NULL)
		strcpy(obj->accType, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "phone");
	if (temp != NULL)
		strcpy(obj->phone, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "mail");
	if (temp != NULL)
		strcpy(obj->mail, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "realName");
	if (temp != NULL)
		strcpy(obj->realName, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "nickName");
	if (temp != NULL)
		strcpy(obj->nickName, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "prcId");
	if (temp != NULL)
		strcpy(obj->prcId, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "sex");
	if (temp != NULL)
		strcpy(obj->sex, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "birthday");
	if (temp != NULL)
		obj->birthday = temp->valueint;
	temp = cJSON_GetObjectItem(json, "country");
	if (temp != NULL)
		strcpy(obj->country, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "province");
	if (temp != NULL)
		strcpy(obj->province, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "city");
	if (temp != NULL)
		strcpy(obj->city, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "district");
	if (temp != NULL)
		strcpy(obj->district, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "address");
	if (temp != NULL)
		strcpy(obj->address, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "headPortrait");
	if (temp != NULL)
		strcpy(obj->headPortrait, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "sign");
	if (temp != NULL)
		strcpy(obj->sign, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "createTime");
	if (temp != NULL)
		obj->createTime = temp->valueint;
	temp = cJSON_GetObjectItem(json, "lastModifyTime");
	if (temp != NULL)
		obj->lastModifyTime = temp->valueint;
	temp = cJSON_GetObjectItem(json, "token");
	if (temp != NULL)
		strcpy(obj->token, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "cmd");
	if (temp != NULL)
		strcpy(obj->cmd, temp->valuestring);
	cJSON_Delete(json);
}

char * UserGenerateJson(User * obj , char * removeField)
{
	cJSON *root = NULL;
	char *out = NULL;
	root = cJSON_CreateObject();
	if (strlen(obj->token) && (!removeField || !strstr(removeField, "token")))
		cJSON_AddStringToObject(root, "token", obj->token);
	if (obj->id != 0 && (!removeField || !strstr(removeField, "id")))
		cJSON_AddNumberToObject(root, "id", obj->id);
	if (strlen(obj->account) && (!removeField || !strstr(removeField, "account")))
		cJSON_AddStringToObject(root, "account", obj->account);
	if (strlen(obj->password) && (!removeField || !strstr(removeField, "password")))
		cJSON_AddStringToObject(root, "password", obj->password);
	if (strlen(obj->accType) && (!removeField || !strstr(removeField, "accType")))
		cJSON_AddStringToObject(root, "accType", obj->accType);
	if (strlen(obj->phone) && (!removeField || !strstr(removeField, "phone")))
		cJSON_AddStringToObject(root, "phone", obj->phone);
	if (strlen(obj->mail) && (!removeField || !strstr(removeField, "mail")))
		cJSON_AddStringToObject(root, "mail", obj->mail);
	if (strlen(obj->realName) && (!removeField || !strstr(removeField, "realName")))
		cJSON_AddStringToObject(root, "realName", obj->realName);
	if (strlen(obj->nickName) && (!removeField || !strstr(removeField, "nickName")))
		cJSON_AddStringToObject(root, "nickName", obj->nickName);
	if (strlen(obj->prcId) && (!removeField || !strstr(removeField, "prcId")))
		cJSON_AddStringToObject(root, "prcId", obj->prcId);
	if (strlen(obj->sex) && (!removeField || !strstr(removeField, "sex")))
		cJSON_AddStringToObject(root, "sex", obj->sex);
	if (obj->birthday && (!removeField || !strstr(removeField, "birthday")))
		cJSON_AddNumberToObject(root, "birthday", obj->birthday);
	if (strlen(obj->country) && (!removeField || !strstr(removeField, "country")))
		cJSON_AddStringToObject(root, "country", obj->country);
	if (strlen(obj->province) && (!removeField || !strstr(removeField, "province")))
		cJSON_AddStringToObject(root, "province", obj->province);
	if (strlen(obj->city) && (!removeField || !strstr(removeField, "city")))
		cJSON_AddStringToObject(root, "city", obj->city);
	if (strlen(obj->district) && (!removeField || !strstr(removeField, "district")))
		cJSON_AddStringToObject(root, "district", obj->district);
	if (strlen(obj->address) && (!removeField || !strstr(removeField, "address")))
		cJSON_AddStringToObject(root, "address", obj->address);
	if (strlen(obj->headPortrait) && (!removeField || !strstr(removeField, "headPortrait")))
		cJSON_AddStringToObject(root, "headPortrait", obj->headPortrait);
	if (strlen(obj->sign) && (!removeField || !strstr(removeField, "sign")))
		cJSON_AddStringToObject(root, "sign", obj->sign);
	if (obj->createTime && (!removeField || !strstr(removeField, "createTime")))
		cJSON_AddNumberToObject(root, "createTime", obj->createTime);
	if (obj->lastModifyTime && (!removeField || !strstr(removeField, "lastModifyTime")))
		cJSON_AddNumberToObject(root, "lastModifyTime", obj->lastModifyTime);
	out = cJSON_Print(root);
	cJSON_Delete(root);
	return out;
}

void updateObj(User * des, User * src)
{
	cJSON *root = NULL;
	char *out = NULL;
	root = cJSON_CreateObject();
	if (strlen(src->password) != 0)
		strcpy(des->password , src->password);
	if (strlen(src->accType) != 0)
		strcpy(des->accType, src->accType);
	if (strlen(src->phone) != 0)
		strcpy(des->phone, src->phone);
	if (strlen(src->mail) != 0)
		strcpy(des->mail, src->mail);
	if (strlen(src->realName) != 0)
		strcpy(des->realName, src->realName);
	if (strlen(src->nickName) != 0)
		strcpy(des->nickName, src->nickName);
	if (strlen(src->prcId) != 0)
		strcpy(des->prcId, src->prcId);
	if (strlen(src->sex) != 0)
		strcpy(des->sex, src->sex);
	if (src->birthday != 0)
		des->birthday = src->birthday;
	if (strlen(src->country) != 0)
		strcpy(des->country, src->country);
	if (strlen(src->province) != 0)
		strcpy(des->province, src->province);
	if (strlen(src->city) != 0)
		strcpy(des->city, src->city);
	if (strlen(src->district) != 0)
		strcpy(des->district, src->district);
	if (strlen(src->address) != 0)
		strcpy(des->address, src->address);
	if (strlen(src->headPortrait) != 0)
		strcpy(des->headPortrait, src->headPortrait);
	if (strlen(src->sign) != 0)
		strcpy(des->sign, src->sign);
	if (src->createTime != 0)
		des->createTime = src->createTime;
	if (src->lastModifyTime != 0)
		des->lastModifyTime = src->lastModifyTime;
	if (strlen(src->token) != 0)
		strcpy(des->token, src->token);
}