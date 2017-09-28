#include <list.h>

list_manage l_m;
//#define LIST_ID_BASE 1000
//队列管理启动
int list_manage_init(void)
{
	memset(&l_m, 0, sizeof(list_manage));
	pthread_spin_init(&l_m.spin_lock, PTHREAD_PROCESS_PRIVATE);
//	l_m.list_id_base = LIST_ID_BASE;
	return 0;
}
//向队列管理中插入新队列
int list_manage_add(list * li)
{
	list * temp = NULL;
	if (li == NULL)
		return err_list_invalid_input_argument;
	pthread_spin_lock(&l_m.spin_lock);
	l_m.count++;					//队列数加一
	temp = l_m.head;				//保存管理队列头指针
	l_m.head = li;					//将新队列挂载在管理队列头部
	l_m.head->next = temp;			//新的头部的next指向老的头部
	pthread_spin_unlock(&l_m.spin_lock);
	return RETURN_OK;
}

//向队列管理中删除队列
int list_manage_del(list * li)
{
	int ret = err_manage_list_del_query_fail;
	if (li == NULL)
		return err_list_invalid_input_argument;
	list * pre = NULL;
	list * curr = NULL;
	pthread_spin_lock(&l_m.spin_lock);
	curr = l_m.head;
	while (curr != NULL)
	{
		if (curr == li )
		{
			if (curr->count != 0)
			{
				ret = err_manage_list_del_have_children_yet;
				break;
			}
			l_m.count--;
			if (pre == NULL)
				l_m.head = l_m.head->next;
			else
				pre->next = curr->next;
			FREE(curr->list_name);
			pthread_spin_destroy(&curr->spin_lock);
			FREE(curr);
			ret = RETURN_OK;
			break;
		}
		else
		{
			pre = curr;
			curr = curr->next;
		}
	}
	pthread_spin_unlock(&l_m.spin_lock);
	return ret;
}
//队列管理关闭
int list_manage_uninit(void)
{
	list * curr = NULL;
	list * temp = NULL;
	curr = l_m.head;
	while (curr != NULL)
	{
		temp = curr;
		curr = curr->next;
		list_destory(temp);
	}
	pthread_spin_destroy(&l_m.spin_lock);
	memset(&l_m, 0, sizeof(list_manage));
	return RETURN_OK;
}

//创建一个队列，并将队列放入管理队列中
list * list_create(char * list_name)
{
	list * list_p = MALLOC(sizeof(list));
	memset(list_p , 0 , sizeof(list));
//	pthread_spin_lock(l_m.spin_lock);
//	list_p->list_id = l_m.list_id_base++;
//	pthread_spin_unlock(l_m.spin_lock);
	list_p->list_name = strdup(list_name);   //可以重名
	pthread_spin_init(&list_p->spin_lock, PTHREAD_PROCESS_PRIVATE);
	list_manage_add(list_p);
	return list_p;
}
//销毁一个队列，并将这个队列移除管理队列
int list_destory(list * li)
{
	list_node * curr = NULL;
	list_node * temp = NULL;

	curr = li->head;
	while (curr != NULL)
	{
		temp = curr;
		curr = curr->next;
		FREE(temp->data);
		FREE(temp);
	}
	li->count = 0;
	list_manage_del(li);
	li = NULL;
	return RETURN_OK;
}
//向一个队列中的头部插入数据
int list_add_to_head(list * li , list_node * node)
{
	node->next = NULL;
	node->prev = NULL;
	pthread_spin_lock(&li->spin_lock);
	node->next = li->head;
	if(li->head != NULL)
		li->head->prev = node;
	li->head = node;
	if (li->count++ == 0)
		li->tail = node;
	pthread_spin_unlock(&li->spin_lock);
	return 0;
}
//向一个队列中的尾部插入数据
int list_add_to_tail(list * li , list_node * node)
{
	node->next = NULL;
	node->prev = NULL;
	pthread_spin_lock(&li->spin_lock);
	if(li->tail != NULL)
		li->tail->next = node;
	node->prev = li->tail;
	li->tail = node;
	if (li->count++ == 0)
		li->head = node;
	pthread_spin_unlock(&li->spin_lock);
	return 0;
}
//从一个队列的头部弹出数据
list_node * list_pop_from_head(list * li)
{
	list_node * temp = NULL;
	pthread_spin_lock(&li->spin_lock);
	if (li->count == 0)
	{
		pthread_spin_unlock(&li->spin_lock);
		return NULL;
	}
	temp = li->head;
	li->head = li->head->next;
	li->head->prev = NULL;
	if (li->count-- == 1)
		li->tail = NULL;
	pthread_spin_unlock(&li->spin_lock);
	temp->next = NULL;
	temp->prev = NULL;
	return temp;
}
//从一个队列的尾部弹出数据
list_node * list_pop_from_tail(list * li)
{
	list_node * temp = NULL;
	pthread_spin_lock(&li->spin_lock);
	if (li->count == 0)
	{
		pthread_spin_unlock(&li->spin_lock);
		return NULL;
	}
	temp = li->tail;
	li->tail = li->tail->prev;
	if(li->tail != NULL)
		li->tail->next = NULL;
	if (li->count-- == 1)
	{
		li->head = NULL;
		li->tail = NULL;
	}
	pthread_spin_unlock(&li->spin_lock);
	temp->next = NULL;
	temp->prev = NULL;
	return temp;
}
 //遍历一个队列中所有节点
list_node * list_next(list * li , bool restart_flag)
{
	list_node * temp = NULL;
	pthread_spin_lock(&li->spin_lock);
	if (li->count == 0)
	{
		pthread_spin_unlock(&li->spin_lock);
		return NULL;
	}
	if (restart_flag == true)
		li->curr = li->head;
	temp = li->curr;
	if (li->curr != NULL)
		li->curr = li->curr->next;
	pthread_spin_unlock(&li->spin_lock);
	return temp;
}

