#ifndef _H_ERROR_
#define _H_ERROR_

#define RETURN_OK	0

//************************************内部错误
#define ERR_INTERNAL_ERROR_START	-1

#define ERR_BASE	-1
#define ERR_PARAM	-2
#define	ERR_THREAD_CREATE_FAIL	-1
#define ERR_CONFIG_FILE_OPEN_FAIL	-2
#define ERR_DATABASE_CONNECT_FAIL	-3
#define ERR_MEMORY_POOL_MALLOC_FAIL	-4
#define ERR_MEMORY_POOL_NOT_INIT	-5
#define ERR_DICT_CREATE_FAIL		-6
#define ERR_LOG_PATH_INVALID		-7
#define	ERR_LOG_LEVEL_INVALID		-8
#define	ERR_LOG_SAVE_DAYS_INVALID	-9
#define ERR_LOG_NOT_INIT			-10
#define ERR_CREATE_EVENTLOOP_FAIL	-11
#define ERR_JSON_PARSE_FAIL			-12
#define ERR_BAD_REQUEST				-13
#define ERR_UNDEFINED_REQUEST_ROUTE	-14
#define ERR_UNDEFINED_REQUEST_TYPE	-15
#define ERR_NOT_SUPPORT_REQUEST		-16
#define ERR_LIST_BASE	-100


#define ERR_UNDEFINED_MSG_GATEWAY		-17
#define ERR_UNDEFINED_CACHE				-18
#define ERR_NOT_FIND_CACHE				-19

#define ERR_USER_REGIST_DUPLICATE		-200
#define ERR_MYSQL_QUERY_FAIL			-101
#define ERR_USER_REGIST_INSERT_FAIL		-102
#define ERR_USER_ERGIST_QUERY_FAIL		-103
#define ERR_USER_DELETE_FAIL			-104
#define ERR_USER_MODIFY_FAIL			-105
#define ERR_USER_MODIFY_ID_INVALID		-106

#define ERR_NOTICE_INSERT_DUPLICATE     -107
#define ERR_NOTICE_INSERT_FAIL          -108
#define ERR_NOTICE_QUERY_FAIL           -109
#define ERR_NOTICE_MODIFY_FAIL          -110
#define ERR_NOTICE_DELETE_FAIL          -111

#define ERR_APP_UPDATE_INSERT_DUPLICATE     -107
#define ERR_APP_UPDATE_INSERT_FAIL          -108
#define ERR_APP_UPDATE_QUERY_FAIL           -109
#define ERR_APP_UPDATE_MODIFY_FAIL          -110
#define ERR_APP_UPDATE_DELETE_FAIL          -112
#define ERR_APP_VERSION_NOT_FOUND_IN_CACHE	-111


#define ERR_INTERNAL_ERROR_END		-200
//************************************内部错误



//****************************************外部错误

#define ERR_PHONE_NUMBER_WRONG			-201
#define ERR_PHONE_NUMBER_DUPLICATE		-202
#define ERR_MSG_CODE_WRONG				-203
#define ERR_INPUT_ARGUMENT_WRONG		-204
#define ERR_PHONE_NUMBER_NOT_REGIST		-205
#define ERR_TOKEN_OUT_OF_DATE			-206
#define ERR_NOT_LOGIN_YET				-207
#define ERR_NO_PERMISSION				-208
#define ERR_ACCOUNT_NOT_REGIST			-209
#define ERR_APPVERSION_NOT_UPDATE		-210
#define ERR_MSG_CODE_OUT_OF_DATE		-211
#define ERR_ACCOUNT_DUPLICATE			-212
#define ERR_NOT_NOTICE_YET				-213

//****************************************外部错误
typedef struct {
	int errcode;
	char errmsg[128];
}errList;

char * gerErrMsg(int code);

char * makeErrorJson(int errcode, char * errmsg);

#endif
