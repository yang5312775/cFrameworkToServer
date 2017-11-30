#ifndef _SERVICE_BASIC_H_
#define _SERVICE_BASIC_H_
#include<socket_server.h>
#include<model.h>
#include"cache.h"
#include"message_gateway.h"
//��ȡ���񲿷����ò���
void getServiceConfig(void);
//������֤ע����ߵ�½��һ��
void verifyBefore(void * prm);
//������֤�ڶ���
void verifyAfter(void * prm);
//�û�����ע�ᣬ���˺�����ע��
void userRegister(void * prm);
//�û���½����Ҫ�˺�����
void userLogin(void * prm);
//��ѯ�û���½״̬
void userStatus(void * prm);
//�û��˳�
void userLogout(void * prm);
//�û���Ϣ�޸ģ���������
void userModify(void * prm);
//�û�ɾ����׼��������Աʹ�ã���ͨ�û�û��ɾ���û�Ȩ��
void userDelete(void * prm);
//��ȡ���͸��û���֪ͨ
void noticeGet(void * prm);
//��ȡ��������汾
void appVer(void * prm);

#endif
