#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>


#define  err_log(errlog) do{perror(errlog); exit(1);}while(0)
#define  N  128

void handler(int sig)
{
	printf("timeout....\n");
}

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


	struct sigaction act;

	sigaction(SIGALRM, NULL, &act);
	act.sa_handler = handler;
	act.sa_flags &= ~SA_RESTART;
	sigaction(SIGALRM, &act, NULL);

	socklen_t  addrlen = sizeof(struct sockaddr);
	int connectfd = 0;

	while(1)
	{

		alarm(5);
		if((connectfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addrlen)) < 0)
		{
			printf("errno = %d \n", errno);
			if(errno == 4)
			{
				printf("accept timeout....\n");
			}
			else
			{
				err_log("fail to accept");
			}
		}	
		printf("Nothing to be done.\n");
	}

	close(sockfd);
	
	return 0;
}
