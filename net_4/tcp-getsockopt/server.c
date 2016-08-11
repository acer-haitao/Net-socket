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


	int optval = 0;
	socklen_t optlen = sizeof(int);



	socklen_t  addrlen = sizeof(struct sockaddr);
	int connectfd = 0;

	if((connectfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addrlen)) < 0)
	{
		err_log("fail to accept");
	}

	printf("Clientaddr:%s --> %d \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	if(getsockopt(connectfd, SOL_SOCKET, SO_SNDBUF, &optval, &optlen ) < 0)
	{
		err_log("fail to getsockopt");
	}

	printf("send buf size: %dK\n", optval/1024);
	
	if(getsockopt(connectfd, SOL_SOCKET, SO_RCVBUF, &optval, &optlen ) < 0)
	{
		err_log("fail to getsockopt");
	}

	printf("recv buf size: %dK\n", optval/1024);



	while(1)
	{
		recv(connectfd, buf, N, 0);
		printf("From client:%s\n", buf);

		strcat(buf, " from server...");
		send(connectfd, buf, N, 0);
	}

	close(sockfd);
	
	return 0;
}
