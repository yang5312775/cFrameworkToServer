#ifndef _APP_UPDATE_H_
#define _APP_UPDATE_H_
#include"model.h"

typedef struct {
	int id;            //�����û�ID    ����
	char type[2]; //����              ����
	char name[20];  //�������            ����
	char version[20];  //����汾           ����
	char level[2]; //��Ҫ�̶�              ����
	char path[256];     //�������λ��     ����
	int  releaseTime;   //����ʱ��         ����
	int  createTime;   //����ʱ��         ����
	int  lastModifyTime; //����޸�ʱ��   ����
	//����Ϊ���ݿ�֮���������Ҫ�����json���������ɹ���
	char token[20];    //��ʱ����
}AppVer;


/**************************���ݿ�����ӿ�**************************************/
//����һ�����ݣ������ֶα�����ֵ���������ʧ��
int AppVerInsert(MYSQL * connection, AppVer * obj);
//��ѯһ�����ݣ��ĸ��ֶ���ֵ���ĸ��ֶξͻ��Ϊ����֮һ
int AppVerQueryReturnItemNum(MYSQL * connection, AppVer * filter);
//��ѯ�������ݣ�filter�ĸ��ֶ���ֵ���ĸ��ֶξ���Ϊ���������ս��ͨ��obj���أ������ɷ���ֵ����
int AppVerQueryrReturnResult(MYSQL * connection, char * sqlManual, AppVer * filter, int limitFrom, int limitCount, AppVer ** obj);
//�޸�һ�����ݣ��ĸ��ֶ���ֵ�����޸��ĸ��ֶΣ��޸�����Ϊ����id
int AppVerModify(MYSQL * connection, AppVer * obj);
//ɾ��һ�����ݣ��ĸ��ֶ���ֵ���ĸ��ֶξͻ��Ϊ����֮һ
int AppVerDelete(MYSQL * connection, AppVer * obj);
/**************************���ݿ�����ӿ�**************************************/



/**********************�ṹ��������json���ݻ�ת�ӿ�*******************************/
//ͨ���ṹ������json����
char * AppVerGenerateJson(AppVer * obj  , char * removeField);
//��json����Ϊ�ṹ������
int AppVerParseJson(char * jsonString, AppVer * obj);
/**********************�ṹ��������json���ݻ�ת�ӿ�*******************************/
#endif