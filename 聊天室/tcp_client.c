/*****************************************************
copyright (C), 2014-2015, Lighting Studio. Co.,     Ltd. 
File name：
Author：Jerey_Jobs    Version:0.1    Date: 
Description：
Funcion List: 
*****************************************************/

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define SERVPORT 5000

struct msg_node
{
    char chart_mode[5];
	int connfd;
	char msg[1000];
};

void* read_thread(void* var)
{
    char recvline[1000];
	int sockfd;
	int n;

	sockfd = *((int*)var);

    while(1)
	{
		n = recvfrom(sockfd,recvline,1000,0,NULL,NULL);
        recvline[n] = '\0';
        fputs(recvline,stdout);
    }

	pthread_exit(NULL);

}

void * write_thread(void * var)
{
    char sendline[1000];
	struct msg_node  msg_text;
	int choice;
	int sockfd;

    sockfd = *((int *)var);

 //   while(fgets(sendline,1000,stdin) != NULL)
   while(1)
	{
		printf("please choice:\n");
		printf("1.  say to all\n");
		printf("2.  say to one\n");

		scanf("%d",&choice);
		getchar();

		printf("please input text:\n");
	//	scanf("%[^\n]",sendline);
	    fgets(sendline,1000,stdin);
	//	getchar();

		if(1 == choice)
		{
			strcpy(msg_text.chart_mode,"stoa");
		}
		else if(2 == choice)
		{
			strcpy(msg_text.chart_mode,"stoo");
			printf("please input number of friend:\n");
			scanf("%d",&msg_text.connfd);
	//		getchar();
		}
		else
		{
			printf("input error!\n");
			exit(-1);
		}

        strcpy(msg_text.msg,sendline);

       // sendto(sockfd,&msg_text,strlen(msg_text),0,(struct sockaddr*)&servaddr,sizeof(servaddr));

		send(sockfd,&msg_text,sizeof(msg_text),0);
	}

	pthread_exit(NULL);
}

int main(int argc,char **argv)
{
    int sockfd;
	int ret;
    struct sockaddr_in servaddr,cliaddr;
	pthread_t tid_read;
	pthread_t tid_write;
  //  char sendline[1000];
 //   char recvline[1000];
//	struct msg_node  msg_text;
//	int choice;

    if(argc != 2)
    {
        printf("usage:client <IP address>\n");
        exit(1);
    }

    sockfd = socket(AF_INET,SOCK_STREAM,0);

    bzero(&servaddr,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(SERVPORT);

    connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	
	ret = pthread_create(&tid_read,NULL,(void*)read_thread,(void*)(&sockfd));

	ret = pthread_create(&tid_write,NULL,(void *)write_thread,(void *)(&sockfd));

	pthread_detach(tid_write);
    pthread_join(tid_read,NULL);

    close(sockfd);

    return 0;
}
