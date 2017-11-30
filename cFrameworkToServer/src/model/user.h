#ifndef _USER_H_
#define _USER_H_
#include"model.h"

typedef struct {
	int id;            //�����û�ID    ����
	char account[20];  //�û��˺�      ����
	char password[20]; //�û�����      ����
	char token[20];    //��ʱ����
	char accType[2];      //�˺����   ����  aΪ����Ա pΪ�ֻ�ע�� nΪ�˺�ע��
	char phone[16];    //�û��绰
	char mail[32];     //�û�����
	char realName[20]; //����
	char nickName[20]; //�ǳ�          ����
	char prcId[20];   //���֤��
	char sex[2];          //�Ա�
	int	 birthday;     //����
	char country[20];  //����
	char province[20]; //ʡ��
	char city[20];     //����
	char district[20]; //��
	char address[256]; //��ϸ��ַ
	char headPortrait[256]; //ͷ���ַ
	char sign[256];      //����ǩ��
	int  createTime;   //ע��ʱ��         ����
	int  lastModifyTime; //����޸�ʱ��   ����
	//����Ϊ���ݿ�֮���������Ҫ�����json���������ɹ���
	int tokenFreshTimeStamp;//token��Ծʱ���
	char cmd[10];      //����ע�ᣬ���Ƿ����¼
}User;


/**************************���ݿ�����ӿ�**************************************/
//����һ�����ݣ������ֶα�����ֵ���������ʧ��
int UserInsert(MYSQL * connection, User * obj);
//��ѯ�����������ĸ��ֶ���ֵ���ĸ��ֶξͻ��Ϊ����֮һ
int UserQueryReturnItemNum(MYSQL * connection, User * filter);
//��ѯ�������ݣ�filter�ĸ��ֶ���ֵ���ĸ��ֶξ���Ϊ���������ս��ͨ��obj���أ������ɷ���ֵ����
int UserQueryrReturnResult(MYSQL * connection, char * sqlManual, User * filter, int limitFrom, int limitCount, User ** obj);
//�޸�һ�����ݣ��ĸ��ֶ���ֵ�����޸��ĸ��ֶΣ��޸�����Ϊ����id
int UserModify(MYSQL * connection, User * filter);
//ɾ��һ�����ݣ��ĸ��ֶ���ֵ���ĸ��ֶξͻ��Ϊ����֮һ
int UserDelete(MYSQL * connection, User * filter);
/**************************���ݿ�����ӿ�**************************************/



/**********************�ṹ��������json���ݻ�ת�ӿ�*******************************/
//ͨ���ṹ������json����
char * UserGenerateJson(User * obj, char * removeField);
//��json����Ϊ�ṹ������
int UserparseJson(char * jsonString, User * obj);
/**********************�ṹ��������json���ݻ�ת�ӿ�*******************************/

//����ĳ�����������
void updateObj(User * des, User * src);

#endif