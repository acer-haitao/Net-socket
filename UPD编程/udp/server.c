#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define  err_log(errlog) do{perror(errlog); exit(1);}while(0)

#define  N  128

int main(int argc, const char *argv[])
{

	int sockfd;
	struct sockaddr_in serveraddr;
	char buf[N] = {};

	if(argc < 3)
	{
		fprintf(stderr, "Usage:%s serverip port.\n", argv[0]);
		return -1;
	}
	if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
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


	while(1)
	{
		recvfrom(sockfd, buf, N, 0, NULL, NULL);
		printf("From client:%s\n", buf);
		if(strncmp(buf, "quit", 4) == 0)
		{
			break;
		}
	}

	close(sockfd);
	
	return 0;
}
