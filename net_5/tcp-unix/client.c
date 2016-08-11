#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <arpa/inet.h>


#define  err_log(errlog) do{perror(errlog); exit(1);}while(0)
#define  N  128

// ./client  192.168.1.196  10000



int main(int argc, const char *argv[])
{
	int sockfd;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;
	char buf[N] = {};


	if((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		err_log("fail to socket");	
	}

	serveraddr.sun_family = AF_UNIX;
	strcpy(serveraddr.sun_path, "mysocket");


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
	}

	printf("client exit...\n");

	close(sockfd);
	
	return 0;
}
