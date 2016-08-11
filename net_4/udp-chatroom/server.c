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
#define  L  1
#define  B  2
#define  Q  3

typedef struct {
	int type;
	char name[N];
	char text[N];
}MSG;

typedef struct node{
	struct sockaddr_in clientaddr;
	struct node *next;
}linknode_t, *linklist;
// typedef struct node * linklist;

linklist create_linklist(void)
{
	linklist h = (linknode_t*)malloc(sizeof(linknode_t));

	h->next = NULL;

	return h;
}

int process_login(int sockfd, MSG msg, linklist h, struct sockaddr_in clientaddr)
{
	sprintf(msg.text, "%s login.", msg.name);
	printf("%s\n", msg.text);

	linklist temp;

	while(h->next != NULL)//先把客户信息放入结构体 然后通过指针操作内部
	{
		sendto(sockfd, &msg, sizeof(msg), 0, (struct sockaddr*)&h->next->clientaddr, sizeof(clientaddr));//?
		h = h->next;
	}

	temp = (linknode_t*)malloc(sizeof(linknode_t));
	temp->clientaddr = clientaddr;

	temp->next = h->next;
	h->next = temp;

	return 0;
}


int main(int argc, const char *argv[])
{

	int sockfd;
	struct sockaddr_in serveraddr;
	struct sockaddr_in clientaddr;
	MSG msg;

	char buf[N] = {};
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

	if(bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		err_log("fail to bind");
	}

	socklen_t addrlen = sizeof(struct sockaddr);

	if((pid = fork()) < 0)
	{
		err_log("fail to fork");
	}
	else if(pid == 0) // system message
	{

	}
	else  // send message to every one
	{
		linklist h = create_linklist();

		while(1)
		{
			recvfrom(sockfd, &msg, sizeof(MSG), 0, (struct sockaddr*)&clientaddr, &addrlen);
			printf("From client:%s --> %d --> %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);
			
			switch(msg.type)
			{
			case L:
				process_login(sockfd, msg, h, clientaddr);
				break;
			case B:
	//			process_broadcast();
				break;
			case Q:
	//			process_quit();
				break;
			}	
		}

	}


	close(sockfd);
	
	return 0;
}
