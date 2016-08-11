#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>



#define  err_log(errlog) do{perror(errlog); exit(1);}while(0)
#define  N  128

// ./client  192.168.1.196  10000

int process_list(struct sockaddr_in serveraddr);
int process_get(char *p , struct sockaddr_in serveraddr);
int process_put(char *p, struct sockaddr_in serveraddr);
int process_help();


int main(int argc, const char *argv[])
{
	char buf[N] = {};
	struct sockaddr_in serveraddr;

	if(argc < 3)
	{
		fprintf(stderr,"Usage:%s serverip port.\n", argv[0]);
		return -1;
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(argv[1]);
	serveraddr.sin_port = htons(atoi(argv[2]));


	while(1)
	{
		printf("<client>");
		fgets(buf, N, stdin);
		buf[strlen(buf)-1] = '\0';

		if(strncmp(buf, "list", 4) == 0)
	    {
			process_list(serveraddr);
		}
		else if(strncmp(buf, "get", 3) == 0) // get filename
		{
			process_get(buf, serveraddr);
		}
		else if(strncmp(buf, "put", 3) == 0)
		{
			process_put(buf, serveraddr);
		}
		else if(strncmp(buf, "help", 4) == 0)
		{
			process_help();	
		}
		else if(strncmp(buf, "quit", 4) == 0)
		{
			break;
		}
		else
		{
			printf("Invalid input.\n");
		}
	}



	return 0;
}

int process_list( struct sockaddr_in serveraddr)
{
	int sockfd;
	char buf[N] = {};

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		err_log("fail to socket");	
	}

	if(connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		err_log("fail to connect");
	}

	strcpy(buf, "L");
	send(sockfd, buf, N, 0);
	
	while(recv(sockfd, buf, N, 0) > 0)
	{
		printf("%s \n", buf);
	}

	printf("list recv done....\n");

	close(sockfd);
	return 0;
}

int process_get(char *p, struct sockaddr_in serveraddr)
{
	int sockfd;
	char buf[N] = {};
	int fd;
	int nbyte = 0;


	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		err_log("fail to socket");	
	}

	if(connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		err_log("fail to connect");
	}

	sprintf(buf, "G%s",p+4);
	send(sockfd, buf, N, 0);

	recv(sockfd, buf, N, 0);
	if(strncmp(buf, "Y", 2) == 0)
	{
		if((fd = open(p+4, O_WRONLY|O_CREAT|O_TRUNC, 0664)) < 0)
		{
			err_log("fail to open");
		}

	}
	else  // N
	{
		printf("%s is not exist!\n", p+4);
		return -1;
	}
	
	while((nbyte = recv(sockfd, buf, N, 0))> 0)
	{
		write(fd, buf, nbyte);
	}

	printf("download done....\n");

	close(sockfd);
	close(fd);

	return 0;
}

int process_put(char *p, struct sockaddr_in serveraddr)
{
	int sockfd;
	char buf[N] = {};
	int fd;
	int nbyte = 0;

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		err_log("fail to socket");	
	}

	if(connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		err_log("fail to connect");
	}

	sprintf(buf, "P%s",p+4);
	send(sockfd, buf, N, 0);

	if((fd = open(p+4, O_RDONLY)) < 0)
	{
		err_log("fail to open");
	}

	
	while((nbyte = read(fd, buf, N))> 0)
	{
		send(sockfd, buf, nbyte, 0);
		usleep(50);
	}

	printf("upload done....\n");

	close(sockfd);
	close(fd);

	return 0;

}

int process_help(void)
{
	printf("*************************************************\n");
	printf("list: list of directory.\n");
	printf("get filename: download filename from server.\n");
	printf("put filename: upload filename to server.\n");
	printf("quit : exit client.\n");
	printf("*************************************************\n");

}
