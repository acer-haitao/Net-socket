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
	struct sockaddr_in groupcastaddr;
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

	groupcastaddr.sin_family = AF_INET;
	groupcastaddr.sin_addr.s_addr = inet_addr(argv[1]);
	groupcastaddr.sin_port = htons(atoi(argv[2]));

	if(bind(sockfd, (struct sockaddr *)&groupcastaddr, sizeof(groupcastaddr)) < 0)
	{
		err_log("fail to bind");
	}

	struct ip_mreq  mreq;
	mreq.imr_multiaddr.s_addr = inet_addr(argv[1]);
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);

	if(setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
	{
		err_log("fail to setsockopt");
	}

	while(1)
	{
		recvfrom(sockfd, buf, N, 0, NULL, NULL);
		printf("From broadcast:%s\n", buf);
		if(strncmp(buf, "quit", 4) == 0)
		{
			break;
		}
	}

	close(sockfd);
	
	return 0;
}
