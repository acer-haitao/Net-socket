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

	if(bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		err_log("fail to bind");
	}

	socklen_t addrlen = sizeof(struct sockaddr);

	while(1)
	{
		recvfrom(sockfd, buf, N, 0, (struct sockaddr*)&clientaddr, &addrlen);
		printf("From client:%s\n", buf);
		printf("Clientaddr: %s \n", clientaddr.sun_path);

		if(strncmp(buf, "quit", 4) == 0)
		{
			break;
		}
		strcat(buf, " from server...");

		sendto(sockfd, buf, N, 0, (struct sockaddr*)&clientaddr, sizeof(clientaddr));
	}

	close(sockfd);
	
	return 0;
}
