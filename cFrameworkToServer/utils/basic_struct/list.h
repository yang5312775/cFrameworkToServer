﻿#ifndef _H_SIMPLEC_LIST
#define _H_SIMPLEC_LIST
#include<basic.h>

//
//	这个万能单链表库, 设计比较老了但可以一用
//	1.使用的时候,需要加上 $LIST_HEAD; 宏
//	2.创建的第一句话就是 list_t head = NULL; 开始从空链表开始list的生涯
//
struct $lnode {
	struct $lnode * next;
};

// 不多说了一定放在想使用链表结构的结构体头部
#define $LIST_HEAD struct $lnode $node

//
// 简单链表结构, 当你使用这个链表的时候 需要 list_t head = NULL; 开始使用之旅
//
typedef void * list_t;

//
// list_next - 主要返回结点n的下一个结点. 用法 node->next = list_next(n) or list_next(n) = node;
// n		: 当前结点
//
#define list_next(n) ((void *)((struct $lnode *)(n))->next)

//
// list_destroy - 链表销毁函数.对于只是栈上数据就不用调这个api
// ph 		: 指向当前链表结点的指针
// die		: 销毁执行的函数
// return	: void
//
extern void list_destroy_(list_t * ph, node_f die);
#define list_destroy(ph, die) \
        list_destroy_((list_t *)ph, (node_f)die)

//
// list_add - 在 cmp(left, x) <= 0 x处前面插入node结点
// ph		: 指向头结点的指针
// cmp		: 比较函数,将left同 *ph中对象按个比较
// left		: cmp(left, x) 比较返回 <=0 or >0
// return	: 返回 SufBase 表示成功!
//
extern int list_add_(list_t * ph, icmp_f cmp, void * left);
#define list_add(ph, cmp, left) \
        list_add_((list_t *)ph, (icmp_f)cmp, left)

//
// list_findpop - 查找到要的结点,并弹出,需要你自己回收
// ph		: 指向头结点的指针
// cmp		: 比较函数,将left同 *ph中对象按个比较
// left		: cmp(left, x) 比较返回 0 >0 <0
// return	: 找到了退出/返回结点, 否则返回NULL
//
extern void * list_findpop_(list_t * ph, icmp_f cmp, const void * left);
#define list_findpop(ph, cmp, left) \
        list_findpop_((list_t *)ph, (icmp_f)cmp, (const void *)(intptr_t)left)

//
// list_find - 链表中查找函数,查找失败返回NULL, 查找成功直接返回那个结点.
// head		: 链表头结点
// cmp		: 查找的比较函数
// left		: cmp(left, right) 用的左结点
// return	: 返回查找的结点对象
//
extern void * list_find(list_t head, icmp_f cmp, const void * left);

//
// list_len - 这里获取当前链表长度, 推荐调用一次就记住len
// h		: 当前链表的头结点
// return	: 返回 链表长度 >=0
//
extern size_t list_len(list_t h);

//
// list_addhead - 采用头查法插入结点, 第一次用需要 list_t head = NULL;
// ph		: 指向头结点的指针
// node		: 待插入的结点对象
// return	: 返回 SufBase 表示成功!
//
extern int list_addhead(list_t * ph, void * node);

//
// list_addtail - 和 list_add 功能相似,但是插入位置在尾巴那
// ph		: 待插入结点的指针
// node		: 待插入的当前结点
// return	: 返回 SufBase 表示成功!
//
extern int list_addtail(list_t * ph, void * node);

//
// list_getidx - 查找索引位置为idx的结点,找不见返回NULL
// head		: 当前结点
// idx		: 查找的索引值[0,len)
// return	: 返回查到的结点
//
extern void * list_getidx(list_t head, int idx);

#endif // !_H_SIMPLEC_LIST