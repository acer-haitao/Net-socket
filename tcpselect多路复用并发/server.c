#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>



#define  err_log(errlog) do{perror(errlog); exit(1);}while(0)
#define  N  128

// ./server  192.168.1.196  10000

int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	fd_set readfds;//定义集合表
	fd_set tempfds;//定义临时集合

	int i = 0;

	char buf[N] = {};

	if(argc < 3)
	{
		fprintf(stderr,"Usage:%s serverip port.\n", argv[0]);
		return -1;
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		err_log("fail to socket");	
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(atoi(argv[2]));

	if(bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		err_log("fail to bind");
	}

	if(listen(sockfd, 5) < 0)
	{
		err_log("fail to listen");
	}


	socklen_t  addrlen = sizeof(struct sockaddr);
	int connectfd = 0;
	int maxfd = 0;//存放最大文件描述符

	FD_ZERO(&readfds);//清除集合
	FD_SET(sockfd, &readfds);//添加到集合中

	maxfd = sockfd;//保存最大文件描述符

	while(1) //如果有客户发送1G影 其他客户等电影传完才能进入
	{
		tempfds = readfds;//每次进入更新集合内数据

	    if(select(maxfd+1, &tempfds, NULL, NULL, NULL) < 0)//监听
		{
			err_log("fail to select");
		}

		for(i = 0; i < maxfd+1; i++)//判断监听对象是否在集合中
		{
			if(FD_ISSET(i, &tempfds))
			{	
				if(i == sockfd)
				{
					if((connectfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addrlen)) < 0)//建立连接的套接字文件 描述符
					{
						err_log("fail to accept");
					}

					printf("Clientaddr:%s --> %d \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

					FD_SET(connectfd, &readfds);//把连接客户connectfd 文件描述加入集合
					maxfd = maxfd > connectfd ? maxfd:connectfd;//更新最大的文件描述符
				}
				else //如果不是sockfd 其他的客户端 内部需要创建线程
				{
					recv(i, buf, N, 0); 
					printf("From client:%s\n", buf);
					if(strncmp(buf, "quit", 4) == 0) //有客户退出
					{
						close(i);//关闭监听对象
						FD_CLR(i, &readfds);//从集合中删除客户
						continue;
					}
					strcat(buf, " from server");//给退出用户发消息
					send(i, buf, N, 0);
				}
			}
		}
	}


	close(sockfd);
	
	return 0;
}
