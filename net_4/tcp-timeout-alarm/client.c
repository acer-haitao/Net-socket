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

// ./client  192.168.1.196  10000



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


	int optval = 0;
	socklen_t optlen = sizeof(int);

	if(getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &optval, &optlen ) < 0)
	{
		err_log("fail to getsockopt");
	}

	printf("send buf size: %dK\n", optval/1024);
	
	if(getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &optval, &optlen ) < 0)
	{
		err_log("fail to getsockopt");
	}

	printf("recv buf size: %dK\n", optval/1024);




	if(connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		err_log("fail to connect");
	}

	while(1)
	{
		fgets(buf, N, stdin);
		buf[strlen(buf)-1] = '\0';

		send(sockfd, buf, N, 0);

		if(strncmp(buf, "quit", 4) == 0)
		{
			break;
		}
		recv(sockfd, buf, N, 0);
		printf("From server:%s\n", buf);
	}

	printf("client exit...\n");

	close(sockfd);
	
	return 0;
}
