#include<socket_basic.h>
char buffer[1024];
void test_socket(void) {


	socket_t server_listener = socket_listen("127.0.0.1", 8888, 20);
	sockaddr_t addr;
	for (;;)
	{
		int newfd = socket_accept(server_listener, &addr);
		if (newfd < 0)
		{
			printf("accept fail!!!error  code is %d\n", newfd);
			return;
		}
		recv(newfd, buffer, 1024, 0);
		send(newfd, buffer, 50, 0);
		socket_close(newfd);
		printf("?");
	}

}