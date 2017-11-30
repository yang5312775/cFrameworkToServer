#ifndef _MESSAGE_GTEWAY_H_
#define _MESSAGE_GTEWAY_H_
#include<basic.h>

#define MSG_WANGYI	1
#define MSG_ALIYUN	2
#define MSG_TENGXUN	3

#define MSG_KEY "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
#define MSG_SECRET "ASDFASDF7998"

typedef struct {
	char code[10];  //��֤��
	int freshTime;  //��Ч����Ϊ��λ
	int timeStamp;  //����ʱ���
}MsgCode;

int msgGatewayCode(int type, char * phoneNumber, int codeBits, char * templateID, MsgCode * returnCode);

int msgGatewayInfo(int type, char * phoneNumber, char * templateID, char * info);
#endif