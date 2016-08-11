#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


#define  err_log(errlog) do{perror(errlog); exit(1);}while(0)

#define  N  128
#define  L  1
#define  B  2
#define  Q  3

typedef struct {
	int type;
	char name[N];
	char text[N];
}MSG;

int main(int argc, const char *argv[])
{

	int sockfd;
	struct sockaddr_in serveraddr;
	char buf[N] = {};
	MSG msg;
	pid_t pid;

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


	socklen_t addrlen = sizeof(struct sockaddr);

	printf("Input name:");
	fgets(buf, N, stdin);
	buf[strlen(buf)-1] = '\0';
	strcpy(msg.name, buf);
	msg.type = L;
	sendto(sockfd, &msg, sizeof(MSG), 0,(struct sockaddr*)&serveraddr, addrlen);

	if((pid = fork()) < 0)
	{
		err_log("fail to fork");
	}
	else if(pid == 0) // child
	{
		while(1)
		{
			fgets(buf, N, stdin);
			buf[strlen(buf)-1] = '\0';
			strcpy(msg.text, buf);
			if(strncmp(buf, "quit", 4) == 0)
			{
				msg.type = Q;
				sendto(sockfd, &msg, sizeof(MSG), 0,(struct sockaddr*)&serveraddr, addrlen);
			
				close(sockfd);
				kill(getppid(), SIGKILL);
				exit(0);
				
			}
			else
			{
				msg.type = B;
				sendto(sockfd, &msg, sizeof(MSG), 0,(struct sockaddr*)&serveraddr, addrlen);

			}

		 }

	}
	else // parent
	{
		while(1)
		{
			recvfrom(sockfd, &msg, sizeof(MSG), 0, NULL, NULL);
			printf("%s\n", msg.text);
		}

	}

	
	close(sockfd);
	
	return 0;
}
