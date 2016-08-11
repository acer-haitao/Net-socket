#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>



#define  err_log(errlog) do{perror(errlog); exit(1);}while(0)
#define  N  128

void handler(int sig)
{
	while(waitpid(-1, NULL, WNOHANG) == 0);//替儿子收尸 防止僵尸进程占用资源
}

int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	char buf[N] = {};
	pid_t pid;

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

	
	signal(SIGCHLD, handler);//?注册信号？


	socklen_t  addrlen = sizeof(struct sockaddr);
	int connectfd = 0;

	while(1)
	{
		if((connectfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addrlen)) < 0)//每接收一次创建新的套接字
		{
			err_log("fail to accept");
		}
		printf("Clientaddr:%s --> %d \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		if((pid = fork()) < 0)//创建父子进程
		{
			err_log("fail to fork");
		}
		else if(pid == 0) // child 子进程处理 收发数据
		{
			close(sockfd);
			while(1)
			{
				recv(connectfd, buf, N, 0);
				printf("From client:%s\n", buf);

				if(strncmp(buf, "quit", 4) == 0)
				{
					break;
				}
				strcat(buf, " from server...");
				send(connectfd, buf, N, 0);
			}
			close(connectfd);
			exit(0);
		}
		else // parent 父进程接待 
		{
			close(connectfd);
		}
  }


	close(sockfd);
	
	return 0;
}
