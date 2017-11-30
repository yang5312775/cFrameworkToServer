#include"message_gateway.h"

int msgWangyiCode(char * key, char * secret , char * phoneNumber, int codeBits, char * templateID, MsgCode * returnCode)
{
	//假实现
	if (codeBits == 4) 
		strcpy(returnCode->code, "8888");
	if (codeBits == 6)
		strcpy(returnCode->code, "888888");
	returnCode->freshTime = 360;
	returnCode->timeStamp = time(0);
	return 0;
}

int msgWangyiInfo(char * key, char * secret , char * phoneNumber, char * templateID, char * info)
{
	//假实现
	return 0;
}


int msgGatewayCode(int type, char * phoneNumber, int codeBits, char * templateID, MsgCode * returnCode)
{
	switch (type)
	{
	case MSG_WANGYI:
		return msgWangyiCode(MSG_KEY, MSG_SECRET, phoneNumber, codeBits, templateID, returnCode);
	case MSG_ALIYUN:
		return msgWangyiCode(MSG_KEY, MSG_SECRET, phoneNumber, codeBits, templateID, returnCode);
	case MSG_TENGXUN:
		return msgWangyiCode(MSG_KEY, MSG_SECRET, phoneNumber, codeBits, templateID, returnCode);
	};
	return ERR_UNDEFINED_MSG_GATEWAY;
}

int msgGatewayInfo(int type, char * phoneNumber, char * templateID, char * info)
{
	switch (type)
	{
	case MSG_WANGYI:
		return msgWangyiInfo(MSG_KEY, MSG_SECRET ,phoneNumber, templateID, info);
	case MSG_ALIYUN:
		return msgWangyiInfo(MSG_KEY, MSG_SECRET ,phoneNumber, templateID, info);
	case MSG_TENGXUN:
		return msgWangyiInfo(MSG_KEY, MSG_SECRET ,phoneNumber, templateID, info);
	};
	return ERR_UNDEFINED_MSG_GATEWAY;
}