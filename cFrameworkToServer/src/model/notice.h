#ifndef _NOTICE_H_
#define _NOTICE_H_
#include"model.h"

typedef struct {
	int id;            //�����û�ID    ����
	char title[128]; //����              ����
	char type[2];  //֪ͨ����            ����    p������֪ͨ   v������֪ͨ
	int startTime; //��ʼʱ��            ����
	int endTime;   //����ʱ��            ����
	char country[20];  //����            ����
	char province[20]; //ʡ�� 
	char city[20];     //����
	char district[20]; //��
	char info[1024]; //����           ����
	int  createTime;   //ע��ʱ��         ����
	int  lastModifyTime; //����޸�ʱ��   ����
	//����Ϊ���ݿ�֮���������Ҫ�����json���������ɹ���
	char token[20];    //��ʱ����
}Notice;


/**************************���ݿ�����ӿ�**************************************/
//����һ�����ݣ������ֶα�����ֵ���������ʧ��
int NoticeInsert(MYSQL * connection, Notice * obj);
//��ѯ�����������ĸ��ֶ���ֵ���ĸ��ֶξͻ��Ϊ����֮һ
int NoticeQueryReturnItemNum(MYSQL * connection, Notice * filter);
//��ѯ�������ݣ�filter�ĸ��ֶ���ֵ���ĸ��ֶξ���Ϊ���������ս��ͨ��obj���أ������ɷ���ֵ����
int NoticeQueryrReturnResult(MYSQL * connection, char * sqlManual, Notice * filter, int limitFrom, int limitCount, Notice ** obj);
//�޸�һ�����ݣ��ĸ��ֶ���ֵ�����޸��ĸ��ֶΣ��޸�����Ϊ����id
int NoticeModify(MYSQL * connection, Notice * obj);
//ɾ��һ�����ݣ��ĸ��ֶ���ֵ���ĸ��ֶξͻ��Ϊ����֮һ
int NoticeDelete(MYSQL * connection, Notice * obj);
/**************************���ݿ�����ӿ�**************************************/



/**********************�ṹ��������json���ݻ�ת�ӿ�*******************************/
//ͨ���ṹ������json����
char * NoticeGenerateJson(Notice * obj, char * removeField);
//��json����Ϊ�ṹ������
int NoticeParseJson(char * jsonString, Notice * obj);
/**********************�ṹ��������json���ݻ�ת�ӿ�*******************************/
#endif