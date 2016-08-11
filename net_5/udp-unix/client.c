#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/un.h>


#define  err_log(errlog) do{perror(errlog); exit(1);}while(0)

#define  N  128

int main(int argc, const char *argv[])
{

	int sockfd;
	struct sockaddr_un serveraddr;
	struct sockaddr_un clientaddr;

	char buf[N] = {};
	if((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
	{
		err_log("fail to socket");
	}

	serveraddr.sun_family = AF_UNIX;
	strcpy(serveraddr.sun_path, "mysocket");

	clientaddr.sun_family = AF_UNIX;
	strcpy(clientaddr.sun_path, "socket");

	if(bind(sockfd, (struct sockaddr*)&clientaddr, sizeof(clientaddr)) < 0)
	{
		err_log("fail to bind");
	}


	socklen_t addrlen = sizeof(struct sockaddr);

	while(1)
	{

		fgets(buf, N, stdin);
		buf[strlen(buf)-1] = '\0';

		sendto(sockfd, buf, N, 0,(struct sockaddr*)&serveraddr, addrlen);
		if(strncmp(buf, "quit", 4) == 0)
		{
			break;
		}

		recvfrom(sockfd, buf, N, 0, NULL, NULL);
		printf("From server:%s\n", buf);
	}

	close(sockfd);
	
	return 0;
}
