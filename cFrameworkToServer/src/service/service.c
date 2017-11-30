#include<socket_server.h>
#include"service.h"



void say_hello(void * prm)
{
	fd_state * state = prm;
#if 1
	char * buffer = malloc(state->buffer_used +1);
	memset(buffer , 0 , state->buffer_used + 1);
	memcpy(buffer , state->buffer , state->buffer_used);
//	printf("%s\n", buffer);
	free(buffer);
	memset(state->buffer , 0 , sizeof(state->buffer));
	strcpy(state->buffer , "sayhello");
	state->buffer_used = strlen("sayhello");
#endif
//	state->errcode = ERR_NOT_SUPPORT_REQUEST;
//	log_print(L_INFO , ".");
}

void userRegist(void * prm)
{
//	real_data_node * node = prm;
//	user uu;


}
//mysql_rollback()   �����ƶ���޸ģ����ִ����mysqlҪ�ع������øú�����


void r_31501(void * prm)
{
	fd_state * state = prm;
	char * buffer = malloc(state->buffer_used + 1);
	memset(buffer, 0, state->buffer_used + 1);
	memcpy(buffer, state->buffer, state->buffer_used);
	printf("%s\n", state->buffer);
	free(buffer);
	memset(state->buffer, 0, sizeof(state->buffer));
	strcpy(state->buffer, "sayhellotoo");
	state->buffer_used = strlen("sayhellotoo");
//	state->errcode = ERR_NOT_SUPPORT_REQUEST;
}


int regist_service(void)
{
	int ret = 0;
	ret = init_function_route();
	if (ret != 0)
		return ret;
	//ע��·�ɻ�������
	add_function_route("verifyBefore", verifyBefore);
	add_function_route("verifyAfter", verifyAfter);
	add_function_route("userRegister", userRegister);
	add_function_route("userLogin", userLogin);
	add_function_route("userStatus", userStatus);
	add_function_route("userLogout", userLogout);
	add_function_route("userModify", userModify);
	add_function_route("userDelete", userDelete);
	add_function_route("noticeGet", noticeGet);
	add_function_route("appVer", appVer);


	//ע��·ҵ�������
	add_function_route("sayhello" , say_hello);
	add_function_route("r_31501", r_31501);

	//��Ҫ�����Ļ���
	createCache();
	//��cache�м��ر�������
	loadCacheNecessary();
	//��ȡ���񲿷����ò���
	getServiceConfig();
	return 0;
}

void destory_service(void)
{
	//����·������
	uninit_function_route();
}
