#include<mysql_basic.h>
#include"notice.h"

static void getWhereOrSetString(char * buffer, Notice * obj, char wos);

int NoticeInsert(MYSQL * connection, Notice * obj)
{
	char sql[SQL_LENGTH];
	int ret = -1;
	MYSQL_RES *result = NULL;
	MYSQL_ROW sqlRow;
	//查询该账号是否可用
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "select * from notice where title='%s';", obj->title);
	result = query_db(sql, connection);
	if (result == NULL)
	{
		ret = ERR_MYSQL_QUERY_FAIL;
		goto end;
	}
	if (result->row_count >= 1)
	{
		ret = ERR_NOTICE_INSERT_DUPLICATE;
		goto end;
	}
	mysql_free_result(result);
	result = NULL;
	//插入该条记录
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "insert into notice(title,type,startTime,endTime,country,province,city,district,info,createTime,lastModifyTime)\
				value('%s','%s','%d','%d','%s','%s','%s','%s','%s','%d','%d');" ,\
	obj->title, obj->type, obj->startTime, obj->endTime, obj->country, obj->province, obj->city, obj->district, obj->info,obj->createTime, obj->lastModifyTime);
	printf("%s", sql);
	ret = exec_db(sql, connection);
	if (ret != 1)
	{
		ret = ERR_NOTICE_INSERT_FAIL;
		goto end;
	}
	//获取插入记录自增ID
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "select id from notice where title='%s';", obj->title);
	result = query_db(sql, connection);
	if (result == NULL)
	{
		ret = ERR_MYSQL_QUERY_FAIL;
		goto end;
	}
	if (result->row_count == 0)
	{
		ret = ERR_NOTICE_QUERY_FAIL;
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

int NoticeQueryReturnItemNum(MYSQL * connection, Notice * filter)
{
	char sql[SQL_LENGTH];
	char arg[SQL_LENGTH];
	int ret = 0;
	MYSQL_RES *result = NULL;
	MYSQL_ROW sqlRow;
	getWhereOrSetString(arg, filter, AND);
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "select count(*) from notice %s;", arg);
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

int NoticeQueryrReturnResult(MYSQL * connection, char * sqlManual , Notice * filter, int limitFrom, int limitCount, Notice ** obj)
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
			sprintf(sql, "select * from notice %s limit %d,%d;", arg, limitFrom, limitCount);
		else
			sprintf(sql, "select * from notice %s ;", arg);
	}else
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
	*obj = malloc(sizeof(Notice) * result->row_count);
	memset(*obj, 0, sizeof(Notice) * result->row_count);
	Notice * p = *obj;
	for (int i = 0; i < result->row_count; i++)
	{
		sqlRow = mysql_fetch_row(result);
		p->id = atoi(sqlRow[0]);
		strcpy(p->title, sqlRow[1]);
		strcpy(p->type, sqlRow[2]);
		p->startTime = atoi(sqlRow[3]);
		p->endTime = atoi(sqlRow[4]);
		strcpy(p->country, sqlRow[5]);
		strcpy(p->province, sqlRow[6]);
		strcpy(p->city, sqlRow[7]);
		strcpy(p->district, sqlRow[8]);
		strcpy(p->info, sqlRow[9]);
		p->createTime = atoi(sqlRow[10]);
		p->lastModifyTime = atoi(sqlRow[11]);
		p++;
	}
	ret = result->row_count;
end:
	if(result != NULL)
		mysql_free_result(result);
	result = NULL;
	return ret;
}

int NoticeModify(MYSQL * connection, Notice * obj)
{
	char sql[SQL_LENGTH];
	char arg[SQL_LENGTH];
	int ret = -1;
	getWhereOrSetString(arg, obj, COMMA);
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "update notice %s where id='%d';", arg, obj->id);
	ret = exec_db(sql, connection);
	if (ret < 0)
		return ERR_NOTICE_MODIFY_FAIL;
	return ret;
}

int NoticeDelete(MYSQL * connection, Notice * obj)
{
	char sql[SQL_LENGTH];
	char arg[SQL_LENGTH];
	int ret = -1;
	getWhereOrSetString(arg, obj, AND);
	memset(sql, 0, SQL_LENGTH);
	sprintf(sql, "delete from notice %s;", arg);
	ret = exec_db(sql, connection);
	if (ret < 0)
		return ERR_NOTICE_DELETE_FAIL;
	return ret;
}

static void getWhereOrSetString(char * buffer, Notice * obj, char wos)
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
	if (strlen(obj->title) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "title='%s'", obj->title);
		strcat(buffer, arg);
		middleFlag = 1;
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
	if (obj->startTime != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "startTime='%d'", obj->startTime);
		strcat(buffer, arg);
		middleFlag = 1;
	}
	if (obj->endTime != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "endTime='%d'", obj->endTime);
		strcat(buffer, arg);
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
	if (strlen(obj->info) != 0)
	{
		if (middleFlag == 1)
			strcat(buffer, flagString);
		memset(arg, 0, SQL_LENGTH);
		sprintf(arg, "info='%s'", obj->info);
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

int NoticeParseJson(char * jsonString, Notice * obj)
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
	temp = cJSON_GetObjectItem(json, "title");
	if (temp != NULL)
		strcpy(obj->title, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "type");
	if (temp != NULL)
		strcpy(obj->type, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "startTime");
	if (temp != NULL)
		obj->startTime = temp->valueint;
	temp = cJSON_GetObjectItem(json, "endTime");
	if (temp != NULL)
		obj->endTime = temp->valueint;
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
	temp = cJSON_GetObjectItem(json, "info");
	if (temp != NULL)
		strcpy(obj->info, temp->valuestring);
	temp = cJSON_GetObjectItem(json, "createTime");
	if (temp != NULL)
		obj->createTime = temp->valueint;
	temp = cJSON_GetObjectItem(json, "lastModifyTime");
	if (temp != NULL)
		obj->lastModifyTime = temp->valueint;
	cJSON_Delete(json);
}

char * NoticeGenerateJson(Notice * obj , char * removeField)
{
	cJSON *root = NULL;
	char *out = NULL;
	root = cJSON_CreateObject();
	if (obj->id && (!removeField || !strstr(removeField, "id")))
		cJSON_AddNumberToObject(root, "id", obj->id);
	if (strlen(obj->title) && (!removeField || !strstr(removeField, "title")))
		cJSON_AddStringToObject(root, "title", obj->title);
	if (strlen(obj->type) && (!removeField || !strstr(removeField, "type")))
		cJSON_AddStringToObject(root, "type", obj->type);
	if (obj->startTime && (!removeField || !strstr(removeField, "startTime")))
		cJSON_AddNumberToObject(root, "startTime", obj->startTime);
	if (obj->endTime && (!removeField || !strstr(removeField, "endTime")))
		cJSON_AddNumberToObject(root, "endTime", obj->endTime);
	if (strlen(obj->country) && (!removeField || !strstr(removeField, "country")))
		cJSON_AddStringToObject(root, "country", obj->country);
	if (strlen(obj->province) && (!removeField || !strstr(removeField, "province")))
		cJSON_AddStringToObject(root, "province", obj->province);
	if (strlen(obj->city) && (!removeField || !strstr(removeField, "city")))
		cJSON_AddStringToObject(root, "city", obj->city);
	if (strlen(obj->district) && (!removeField || !strstr(removeField, "district")))
		cJSON_AddStringToObject(root, "district", obj->district);
	if (strlen(obj->info) && (!removeField || !strstr(removeField, "info")))
		cJSON_AddStringToObject(root, "info", obj->info);
	if (obj->createTime && (!removeField || !strstr(removeField, "createTime")))
		cJSON_AddNumberToObject(root, "createTime", obj->createTime);
	if (obj->lastModifyTime && (!removeField || !strstr(removeField, "lastModifyTime")))
		cJSON_AddNumberToObject(root, "lastModifyTime", obj->lastModifyTime);
	out = cJSON_Print(root);
	cJSON_Delete(root);
	return out;
}

