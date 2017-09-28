#ifndef _H_SIMPLEC_LIST
#define _H_SIMPLEC_LIST
#include<basic.h>

enum list_flag {

	err_list_base = ERR_LIST_BASE,
	err_list_invalid_input_argument,   //-99
	err_manage_list_del_query_fail,   //-99
	err_manage_list_del_have_children_yet,   //-99
};

typedef struct list_node list_node;
struct list_node {
	char * data;
	list_node * next;
	list_node * prev;
};

typedef struct list list;
struct list {
	char * list_name;
//	int list_id;
	int	count;
	pthread_spinlock_t  spin_lock;           
	list_node * head;
	list_node * tail;
	list_node * curr;
	list * next;
};

typedef struct {
	int count;
//	int list_id_base;
	pthread_spinlock_t  spin_lock;
	list * head;
}list_manage;

//队列管理启动
int list_manage_init(void);

//向队列管理中插入新队列
int list_manage_add(list * li);


//向队列管理中删除队列
int list_manage_del(list * li);

//队列管理关闭
int list_manage_uninit(void);

//创建一个队列，并将队列放入管理队列中
list * list_create(char * list_name);

//销毁一个队列，并将这个队列移除管理队列
int list_destory(list * li);

//向一个队列中的头部插入数据
int list_add_to_head(list * li, list_node * node);

//向一个队列中的尾部插入数据
int list_add_to_tail(list * li, list_node * node);

//从一个队列的头部弹出数据
list_node * list_pop_from_head(list * li);

//从一个队列的尾部弹出数据
list_node * list_pop_from_tail(list * li);

//遍历一个队列中所有节点
list_node * list_next(list * li, bool restart_flag);



#endif