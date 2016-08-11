#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>


#define  err_log(errlog) do{perror(errlog); exit(1);}while(0)
#define  N  128

// ./server  192.168.1.196  10000

int process_list(int connectfd);
int process_get(int connectfd, char *p);
int process_put(int connectfd, char *p);


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


	socklen_t  addrlen = sizeof(struct sockaddr);
	int connectfd = 0;

	while(1)
	{
		if((connectfd = accept(sockfd, (struct sockaddr*)&clientaddr, &addrlen)) < 0)
		{
			err_log("fail to accept");
		}

		printf("Clientaddr:%s --> %d \n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

		recv(connectfd, buf, N, 0);

		switch(buf[0])
		{
		case 'L':
			process_list(connectfd);
			break;
		case 'P':
			process_put(connectfd, buf);
			break;
		case 'G':
			process_get(connectfd, buf);  //Gfilename
			break;
		default:
			printf("Invalid data...\n");
		}


	}

	close(sockfd);
	
	return 0;
}

int process_list(int connectfd)
{
	DIR *dir;
	struct dirent *dp;
	char buf[N] = {};

	if((dir = opendir(".")) == NULL)
	{
		err_log("fail to opendir");
	}
	while((dp = readdir(dir)) != NULL)
	{
		if(dp->d_name[0] == '.')
		{
			continue;
		}
		strcpy(buf, dp->d_name);
		send(connectfd, buf, N, 0);
		usleep(50);
	}

	printf("list send done ...\n");
	closedir(dir);
	close(connectfd);

	return 0;
}

//Gfilename
int process_get(int connectfd, char *p)
{
	int fd = 0;
	char buf[N] = {};
	int nbyte = 0;

	if((fd = open(p+1, O_RDONLY)) < 0)
	{
		strcpy(buf, "N");
		send(connectfd, buf, N, 0);
		return -1;
	}
	else
	{
		strcpy(buf, "Y");
		send(connectfd, buf, N, 0);
	}

	while((nbyte = read(fd, buf, N)) > 0)
	{
		send(connectfd, buf, nbyte, 0);
		usleep(50);
	}
	printf("transmit done...\n");

	close(fd);
	close(connectfd);

	return  0;
}

int process_put(int connectfd, char *p)
{
	int fd = 0;
	char buf[N] = {};
	int nbyte = 0;

	if((fd = open(p+1, O_WRONLY|O_CREAT|O_TRUNC, 0664)) < 0)
	{
		err_log("fail to open");
	}
	while((nbyte = recv(connectfd, buf, N, 0)) > 0)
	{
		write(fd, buf, nbyte);
	}
	printf("recv done...\n");

	close(fd);
	close(connectfd);

	return  0;
}
