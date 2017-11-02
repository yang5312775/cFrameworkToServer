#include"function_route.h"

dict * function_route_dict = NULL;



int init_function_route(void)
{
	function_route_dict = dictCreateNoFreeValue();
	if (function_route_dict == NULL)
		return ERR_DICT_CREATE_FAIL;
	return 0;
}

int add_function_route(char * key , route_fun * fun)
{
	return dictReplace(function_route_dict , key , fun);
}

int delete_function_route(char * key)
{
	return dictDelete(function_route_dict , key);
}
route_fun *  get_function_route(char * key)
{
	return dictFetchValue(function_route_dict, key);
}

void uninit_function_route(void)
{
	dictRelease(function_route_dict);
}


