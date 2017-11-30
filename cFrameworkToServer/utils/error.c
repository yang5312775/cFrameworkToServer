#include"error.h"
#include"c_json.h"

#define UNKNOWN_ERR_CODE "unknown error code"
#define INTERNAL_ERROR "server internal errpr ,plese notify  administrator!"

errList errlist[] = {
	{ RETURN_OK								, "ok" },
	{ ERR_PHONE_NUMBER_WRONG				, "wrong phone number format"},
	{ ERR_PHONE_NUMBER_DUPLICATE			, "phone number  duplicate" },
	{ ERR_MSG_CODE_WRONG					, "message verification fail" },
	{ ERR_INPUT_ARGUMENT_WRONG				, "input argument wrong" },
	{ ERR_PHONE_NUMBER_NOT_REGIST			, "phone number not regist yet" },
	{ ERR_TOKEN_OUT_OF_DATE					, "token out of date" },
	{ ERR_NOT_LOGIN_YET						, "user not login yet" },
	{ ERR_NO_PERMISSION						, "user not permission to delete operation" },
	{ ERR_ACCOUNT_NOT_REGIST				, "account not regist yet" },
	{ ERR_APPVERSION_NOT_UPDATE				, "app version not update yet" },
	{ ERR_MSG_CODE_OUT_OF_DATE				, "message code out of date" },
	{ ERR_ACCOUNT_DUPLICATE					, "account duplicate" },
	{ ERR_NOT_NOTICE_YET					, "not notice yet" },
};


char * gerErrMsg(int code)
{
	if (code < ERR_INTERNAL_ERROR_START && code > ERR_INTERNAL_ERROR_END)
		return INTERNAL_ERROR;

	int size = sizeof(errlist)/sizeof(errList);
	for (int i = 0; i < size; i++)
	{
		if (errlist[i].errcode == code)
			return errlist[i].errmsg;
	}
	return UNKNOWN_ERR_CODE;
}

char * makeErrorJson(int errcode, char * errmsg)
{
	cJSON *root;
	root = cJSON_CreateObject();
	cJSON_AddNumberToObject(root, "errcode", errcode);
	cJSON_AddStringToObject(root, "errmsg", errmsg);
	char *out = cJSON_Print(root);
	cJSON_Delete(root);
	return out;
}