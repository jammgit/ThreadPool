/*
*	作者：江伟霖
*	时间：2016/04/15
*	邮箱：269337654@qq.com or adalinors@gmail.com
*	描述：简单的服务器回射类
*/

#ifndef ECHO_H
#define ECHO_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

class Echo
{
public:
	Echo(){};
	Echo(int connfd):m_connfd(connfd){};
	void SetConnfd(int connfd)
	{
		m_connfd = connfd;
	}
	void Process();
private:
	int m_connfd;
	const int BUFFSIZE = 256;
};

void Echo::Process()
{
	int nRead;
	char buf[BUFFSIZE];
	char sendbuf[BUFFSIZE + 32];
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		nRead = recv(m_connfd, buf, BUFFSIZE, 0);
		if (nRead <= 0)
		{
			close(m_connfd);
			break;
		}
		
		buf[nRead] = '\0';
		//memset(sendbuf, 0, sizeof(sendbuf));
		//sprintf(sendbuf, "You have send to server:%s", buf);
		send(m_connfd, buf, strlen(buf), 0);
	}
}


#endif