#include"service.h"

void say_hello(void * prm)
{
	real_data_node * node = prm;
	free(node->data);
	node->data = strdup("sayhello");
	node->len = strlen("sayhello");
//	printf("say hello\n");
}




int regist_service(void)
{
	int ret = 0;
	ret = init_function_route();
	if (ret != 0)
		return ret;
	add_function_route("sayhello" , say_hello);

	return 0;
}

void destory_service(void)
{
	//方法路由销毁
	uninit_function_route();
}
