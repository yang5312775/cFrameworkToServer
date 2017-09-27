﻿#include <list.h>


#if 0
#undef  list_next
#define list_next(n) ((struct $lnode *)(n))->next

//
// list_destroy - 链表销毁函数.对于只是栈上数据就不用调这个api
// ph 		: 指向当前链表结点的指针
// die		: 销毁执行的函数
// return	: void
//
void
list_destroy_(list_t * ph, node_f die) {
	struct $lnode * head;
	if ((!ph) || !(head = *ph))
		return;

	if (die) {
		do {
			struct $lnode * next = head->next;
			die(head);
			head = next;
		} while (head);
	}

	*ph = NULL;
}

//
// list_add - 在 cmp(left, x) <= 0 x处前面插入node结点
// ph		: 指向头结点的指针
// cmp		: 比较函数,将left同 *ph中对象按个比较
// left		: cmp(left, x) 比较返回 <=0 or >0
// return	: 返回 SufBase 表示成功!
//
int 
list_add_(list_t * ph, icmp_f cmp, void * left) {
	struct $lnode * head;
	DEBUG_CODE({
		if (!ph || !cmp || !left) {
			RETURN(ErrParam, "check ph=%p, cmp=%p, left=%p.", ph, cmp, left);
		}
	});

	head = *ph;
	// 插入为头结点直接返回
	if (!head || cmp(left, head) <= 0) {
        list_next(left) = head;
		*ph = left;
		return SufBase;
	}

	// 中间插入了
	while (head->next) {
		if (cmp(left, head->next) <= 0)
			break;
		head = head->next;
	}
	list_next(left) = head->next;
	head->next = left;
	return SufBase;
}

//
// list_findpop - 查找到要的结点,并弹出,需要你自己回收
// ph		: 指向头结点的指针
// cmp		: 比较函数,将left同 *ph中对象按个比较
// left		: cmp(left, x) 比较返回 0 >0 <0
// return	: 找到了退出/返回结点, 否则返回NULL
//
void * 
list_findpop_(list_t * ph, icmp_f cmp, const void * left) {
	struct $lnode * head, * tmp;
	if((!ph) || (!cmp) || (!left) || !(head = *ph)){
		RETURN(NULL, "check find {(!ph) || (!cmp) || (!left) || !(head = *ph)}!");
	}

	// 头部检测
	if(cmp(left, head) == 0){
		*ph = head->next;
		return head;
	}

	// 后面就是普通的查找
	while((tmp = head->next)){
		if(cmp(left, tmp) == 0){
			head->next = tmp->next;
			break;
		}
		head = tmp;
	}
	
	return tmp;
}

//
// list_find - 链表中查找函数,查找失败返回NULL, 查找成功直接返回那个结点.
// head		: 链表头结点
// cmp		: 查找的比较函数
// left		: cmp(left, right) 用的左结点
// return	: 返回查找的结点对象
//
void *
list_find(list_t head, icmp_f cmp, const void * left) {
	if (cmp == NULL || left == NULL) {
		RETURN(NULL, "check(cmp == NULL || left == NULL)!");
	}

	//找到结果直接结束
	while (!!(head)) {
		if (cmp(left, head) == 0)
			break;
		head = list_next(head);
	}

	return head;
}

//
// list_len - 这里获取当前链表长度, 推荐调用一次就记住len
// h		: 当前链表的头结点
// return	: 返回 链表长度 >=0
//
size_t
list_len(list_t h) {
	size_t len = 0;
	while(h){
		++len;
		h = list_next(h);
	}
	return len;
}

//
// list_addhead - 采用头查法插入结点, 第一次用需要 list_t head = NULL;
// ph		: 指向头结点的指针
// node		: 待插入的结点对象
// return	: 返回 SufBase 表示成功!
//
inline int 
list_addhead(list_t * ph, void * node) {
	if (!ph || !node){
		RETURN(ErrParam, "check (pal == %p || node == %p)!", ph, node);
	}

    list_next(node) = *ph;
	*ph = node;

	return SufBase;
}

//
// list_addtail - 和 list_add 功能相似,但是插入位置在尾巴那
// ph		: 待插入结点的指针
// node		: 待插入的当前结点
// return	: 返回 SufBase 表示成功!
//
int
list_addtail(list_t * ph, void * node) {
	struct $lnode * head;
	if (!ph || !node) {
		RETURN(ErrParam, "check (pal == %p || node == %p)!", ph, node);
	}

	list_next(node) = NULL;//将这个结点的置空
	if (!(head = *ph)) { //插入的是头结点直接返回
		*ph = node;
		return SufBase;
	}

	while (!!(head->next))
		head = head->next;
	head->next = node;

	return SufBase;
}

//
// list_getidx - 查找索引位置为idx的结点,找不见返回NULL
// head		: 当前结点
// idx		: 查找的索引值[0,len)
// return	: 返回查到的结点
//
void * 
list_getidx(list_t head, int idx) {
	if(head == NULL || idx < 0) {
		RETURN(NULL, "check is h == %p || idx = %d.", head, idx);
	}

	// 主要查找函数,代码还是比较精简的还是值得学习的
	while(head){
		if (idx-- == 0)
			break;
		head = list_next(head);
	}
	
	return head;
}
#endif