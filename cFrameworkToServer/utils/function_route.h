#ifndef _FUNCTION_ROUTE_H_
#define _FUNCTION_ROUTE_H_
#include<basic.h>

typedef void route_fun(void * prm);

int init_function_route(void);

int add_function_route(char * key, route_fun * fun);

int delete_function_route(char * key);

route_fun *  get_function_route(char * key);

void uninit_function_route(void);


#endif
