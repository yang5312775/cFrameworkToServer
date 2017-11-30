#include<mysql_basic.h>
#include"appVersion.h"

static void getWhereOrSetString(char * buffer, AppVer * obj, char wos);

int AppVerInsert(MYSQL * connection, AppVer * obj)
{
	char sql[SQL_LENGTH];
	int ret = -1;
	MYSQL_RES *result = NULL;
	MYSQL_ROW sqlRow;
	//查询该账号是否可用
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "select * from appver where name='%s';", obj->name);
	result = query_db(sql, connection);
	if (result == NULL)
	{
		ret = ERR_MYSQL_QUERY_FAIL;
		goto end;
	}
	if (result->row_count >= 1)
	{
		ret = ERR_APP_UPDATE_INSERT_DUPLICATE;
		goto end;
	}
	mysql_free_result(result);
	result = NULL;
	//插入该条记录
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "insert into appver(type,name,version,level,path,releaseTime,createTime,lastModifyTime)\
				value('%s','%s','%s','%s','%s','%d','%d','%d');", \
		obj->type, obj->name, obj->version, obj->level, obj->path, obj->releaseTime , obj->createTime, obj->lastModifyTime);
//	printf("%s", sql);
	ret = exec_db(sql, connection);
	if (ret != 1)
	{
		ret = ERR_APP_UPDATE_INSERT_FAIL;
		goto end;
	}
	//获取插入记录自增ID
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "select id from appver where name='%s';", obj->name);
	result = query_db(sql, connection);
	if (result == NULL)
	{
		ret = ERR_MYSQL_QUERY_FAIL;
		goto end;
	}
	if (result->row_count == 0)
	{
		ret = ERR_APP_UPDATE_QUERY_FAIL;
		goto end;
	}
	sqlRow = mysql_fetch_row(result);
	obj->id = atoi(sqlRow[0]);
end:
	if (result != NULL)
		mysql_free_result(result);
	result = NULL;
	return ret;
}

int AppVerQueryReturnItemNum(MYSQL * connection, AppVer * filter)
{
	char sql[SQL_LENGTH];
	char arg[SQL_LENGTH];
	int ret = 0;
	MYSQL_RES *result = NULL;
	MYSQL_ROW sqlRow;
	getWhereOrSetString(arg, filter, AND);
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "select count(*) from appver %s;", arg);
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

int AppVerQueryrReturnResult(MYSQL * connection, char * sqlManual ,AppVer * filter, int limitFrom, int limitCount, AppVer ** obj)
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
			sprintf(sql, "select * from appver %s limit %d,%d;", arg, limitFrom, limitCount);
		else
			sprintf(sql, "select * from appver %s ;", arg);
	}
	else
		strcpy(sql , sqlManual);

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
	*obj = malloc(sizeof(AppVer) * result->row_count);
	memset(*obj, 0, sizeof(AppVer) * result->row_count);
	AppVer * p = *obj;
	for (int i = 0; i < result->row_count; i++)
	{
		sqlRow = mysql_fetch_row(result);
		p->id = atoi(sqlRow[0]);
		strcpy(p->name, sqlRow[1]);
		strcpy(p->type, sqlRow[2]);
		strcpy(p->version, sqlRow[3]);
		strcpy(p->level, sqlRow[4]);
		strcpy(p->path, sqlRow[5]);
		p->releaseTime = atoi(sqlRow[6]);
		p->createTime = atoi(sqlRow[7]);
		p->lastModifyTime = atoi(sqlRow[8]);
		p++;
	}
	ret = result->row_count;
end:
	if (result != NULL)
		mysql_free_result(result);
	result = NULL;
	return ret;
}

int AppVerModify(MYSQL * connection, AppVer * obj)
{
	char sql[SQL_LENGTH];
	char arg[SQL_LENGTH];
	int ret = -1;
	getWhereOrSetString(arg, obj, COMMA);
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "update appver %s where id='%d';", arg, obj->id);
	ret = exec_db(sql, connection);
	if (ret < 0)
		return ERR_APP_UPDATE_MODIFY_FAIL;
	return ret;
}

int AppVerDelete(MYSQL * connection, AppVer * obj)
{
	char sql[SQL_LENGTH];
	char arg[SQL_LENGTH];
	int ret = -1;
	getWhereOrSetString(arg, obj, AND);
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "delete from appver %s;", arg);
	ret = exec_db(sql, connection);
	if (ret < 0)
		return ERR_APP_UPDATE_DELETE_FAIL;
	return ret;
}

static void getWhereOrSetString(char * buffer, AppVer * obj, char wos)
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
	}
	if (strlen(obj->type) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "type='%s'", obj->type);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->name) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "name='%s'", obj->name);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->version) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "version='%s'", obj->version);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->level) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "level='%s'", obj->level);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (strlen(obj->path) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "path='%s'", obj->path);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (obj->releaseTime != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "releaseTime='%d'", obj->releaseTime);
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
		strcat(buffer, " ");
	}
}

int AppVerParseJson(char * jsonString, AppVer * obj)
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
	temp = cJSON_GetObjectItem(json, "token");
	if (temp != NULL)
		strcpy(obj->token, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "type");
	if (temp != NULL)
		strcpy(obj->type, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "name");
	if (temp != NULL)
		strcpy(obj->name, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "version");
	if (temp != NULL)
		strcpy(obj->version, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "level");
	if (temp != NULL)
		strcpy(obj->level, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "path");
	if (temp != NULL)
		strcpy(obj->path, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "releaseTime");
	if (temp != NULL)
		obj->releaseTime = temp->valueint;
	temp = cJSON_GetObjectItem(json, "createTime");
	if (temp != NULL)
		obj->createTime = temp->valueint;
	temp = cJSON_GetObjectItem(json, "lastModifyTime");
	if (temp != NULL)
		obj->lastModifyTime = temp->valueint;
	cJSON_Delete(json);
}

char * AppVerGenerateJson(AppVer * obj , char * removeField)
{
	cJSON *root = NULL;
	char *out = NULL;
	root = cJSON_CreateObject();
	if (obj->id  && (!removeField || !strstr(removeField, "id")))
		cJSON_AddNumberToObject(root, "id", obj->id);
	if (strlen(obj->name) && (!removeField || !strstr(removeField, "name")))
		cJSON_AddStringToObject(root, "name", obj->name);
	if (strlen(obj->type) && (!removeField || !strstr(removeField, "type")))
		cJSON_AddStringToObject(root, "type", obj->type);
	if (strlen(obj->version) && (!removeField || !strstr(removeField, "version")))
		cJSON_AddStringToObject(root, "version", obj->version);
	if (strlen(obj->level) && (!removeField || !strstr(removeField, "level")))
		cJSON_AddStringToObject(root, "level", obj->level);
	if (strlen(obj->path) && (!removeField || !strstr(removeField, "path")))
		cJSON_AddStringToObject(root, "path", obj->path);
	if (obj->releaseTime && (!removeField || !strstr(removeField, "releaseTime")))
		cJSON_AddNumberToObject(root, "releaseTime", obj->releaseTime);
	if (obj->createTime && (!removeField || !strstr(removeField, "createTime")))
		cJSON_AddNumberToObject(root, "createTime", obj->createTime);
	if (obj->lastModifyTime && (!removeField || !strstr(removeField, "lastModifyTime")))
		cJSON_AddNumberToObject(root, "lastModifyTime", obj->lastModifyTime);
	out = cJSON_Print(root);
	cJSON_Delete(root);
	return out;
}

