/*
*	作者：江伟霖
*	时间：2016/04/16
*	邮箱：269337654@qq.com or adalinors@gmail.com
*	描述：简单回射服务器主程序
*/


#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "ThreadPool.h"
#include "Echo.h"

int main(int argc, char *argv[])
{
	int listenfd;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0)
	{
		printf("socket error\n");
		exit(-1);
	}

	struct sockaddr_in serv;
	serv.sin_family = AF_INET;
	serv.sin_port = htons(7777);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);

	int ret = bind(listenfd, (struct sockaddr *)&serv, sizeof(serv));
	if (ret < 0)
	{
		printf("bind error\n");
		exit(-1);
	}
	listen(listenfd, 32767);
	/* 创建线程池 */
	ThreadPool<Echo> Tpool;

	const int MAX_EVENT_NUM = 1000;
	/* 想内核注册监听套接字的EPOLLIN事件 */
	int epollfd = epoll_create(5);
	
	epoll_event event[MAX_EVENT_NUM];
	epoll_event e;
	e.data.fd = listenfd;
	e.events = EPOLLIN;
	epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &e);


	for (;;)
	{
		int number = epoll_wait(epollfd, event, MAX_EVENT_NUM, -1);
		if (number < 0 && errno == EINTR)
		{
			printf("epoll_wait error\n");
			exit(-1);
		}

		for (int i = 0; i < number; ++i)
		{
			int fd = event[i].data.fd;
			if (fd == listenfd && (event[i].events & EPOLLIN))
			{
				int connfd = accept(listenfd, NULL, NULL);
				/* 当前只是连接，并没有请求 */
				epoll_event e;
				e.data.fd = connfd;
				e.events = EPOLLIN | EPOLLET;
				epoll_ctl(epollfd, EPOLL_CTL_ADD, connfd, &e);
			}
			else if (event[i].events & EPOLLIN)
			{
				/* 这里必须是new的哟，不熟悉list容器，被坑了一把 */
				Echo *preq = new Echo(fd);
				Tpool.Append(preq);
				epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
			}

		}

	}


	close(listenfd);

	return 0;
}
