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
	struct sockaddr_in broadcastaddr;
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

	broadcastaddr.sin_family = AF_INET;
	broadcastaddr.sin_addr.s_addr = inet_addr(argv[1]); // 192.168.1.255
	broadcastaddr.sin_port = htons(atoi(argv[2]));

	int on = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)) < 0)
	{
		err_log("fail to setsockopt");
	}

	socklen_t addrlen = sizeof(struct sockaddr);

	while(1)
	{

		fgets(buf, N, stdin);
		buf[strlen(buf)-1] = '\0';

		sendto(sockfd, buf, N, 0,(struct sockaddr*)&broadcastaddr, addrlen);
		if(strncmp(buf, "quit", 4) == 0)
		{
			break;
		}

	}

	close(sockfd);
	
	return 0;
}
