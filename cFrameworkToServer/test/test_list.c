#include<list.h>

//
// 测试 list.h 接口的使用
//
void test_list(void) {

	printf("test test_list start!!!\n");
	time_t oldTime = time(0);
	int i = 0;
	do {
		list_manage_init();
		i++;
		list * l1 = list_create("LIST1");
		list * l2 = list_create("LIST2");
		list * l3 = list_create("LIST3");
		list_node * node = (list_node *)MALLOC(sizeof(list_node));
		node->data = MALLOC(20);
		strcpy(node->data, "hello1");
		list_add_to_head(l1, node);

		list_node * node12 = (list_node *)MALLOC(sizeof(list_node));
		node12->data = MALLOC(20);
		strcpy(node12->data, "hello12");
		list_add_to_head(l2, node12);

		list_node * node13 = (list_node *)MALLOC(sizeof(list_node));
		node13->data = MALLOC(20);
		strcpy(node13->data, "hello13");
		list_add_to_head(l3, node13);

		list_node * node2 = (list_node *)MALLOC(sizeof(list_node));
		node2->data = MALLOC(20);
		strcpy(node2->data, "hello2");
		list_add_to_head(l1, node2);
		list_node * node22 = (list_node *)MALLOC(sizeof(list_node));
		node22->data = MALLOC(20);
		strcpy(node22->data, "hello22");
		list_add_to_head(l2, node22);

		list_node * node3 = (list_node *)MALLOC(sizeof(list_node));
		node3->data = MALLOC(20);
		strcpy(node3->data, "hello3");
		list_add_to_head(l1, node3);

		list_node * node4 = (list_node *)MALLOC(sizeof(list_node));
		node4->data = MALLOC(20);
		strcpy(node4->data, "hello4");
		list_add_to_head(l1, node4);
		list_node * node5 = (list_node *)MALLOC(sizeof(list_node));
		node5->data = MALLOC(20);
		strcpy(node5->data, "hello5");
		list_add_to_tail(l1, node5);
		list_node * node52 = (list_node *)MALLOC(sizeof(list_node));
		node52->data = MALLOC(20);
		strcpy(node52->data, "hello52");
		list_add_to_tail(l2, node52);
		if (time(0) != oldTime)
		{
			oldTime = time(0);
			MALLOC_PRINT();
		}
		list_node * node6 = list_pop_from_head(l1);
		FREE(node6->data);
		FREE(node6);

		list_node * node7 = list_pop_from_tail(l1);
		FREE(node7->data);
		FREE(node7);
		if (time(0) != oldTime)
		{
			oldTime = time(0);
			MALLOC_PRINT();
		}
		list_destory(l2);
		list_destory(l3);
		list_destory(l1);


		list_manage_uninit();
//		Sleep(10);
//		printf("loop[%d]\n" , i);

	} while (1);

	



	//	Sleep(100);
	printf("test test_list end!!!\n");
}