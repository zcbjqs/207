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
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define SERVPORT 5000

typedef struct online_client
{
    int connfd;
	//struct sockaddr_in cliaddr;
	
	struct online_client *next;
} Online;

struct thread_node
{
    int connfd;
	Online * head;
};

struct msg_node
{
    char chart_mode[5];
	int connfd;
	char msg[1000];
};

void is_malloc_ok(Online * new_node)
{
	if(new_node == NULL)
	{
		printf("malloc error!\n");
		exit(0);
	}
}

void create_new_node(Online ** new_node)
{
	(*new_node) = (Online *)malloc(sizeof(Online));
	is_malloc_ok(*new_node);
}

void create_link(Online ** head)
{
	create_new_node(head);
	(*head)->next = NULL;
}

void insert_node_head(Online *head,Online *new_node)
{
    new_node->next = head->next;
	head->next = new_node;
}

void release_link(Online ** head)
{
   Online * temp;
   temp = *head;

   while(*head != NULL)
    {   
        *head = temp->next;
	    free(temp);
		temp = *head;
    }
}

void visit_link_one(struct thread_node * connfd_node,struct msg_node * msg_one)
{
	int n;

    Online * tmp;

    tmp = connfd_node->head->next;
         	
    n = strlen(msg_one->msg);

    while(tmp != NULL && tmp->connfd != msg_one->connfd)
   {
    //	sendto(connfd,msg,n,0,
	//			(struct sockaddr*)&cliaddr,sizeof(cliaddr));
	  //  send(tmp->connfd,msg,n,0);
        tmp = tmp->next;
   }

	if(tmp != NULL)
	{
		send(tmp->connfd,msg_one->msg,n,0);
	}
	else
	{
		printf("client is offline!\n");
	}
}

void visit_link_all(Online *head,char msg[])
{
	int n;

    Online * tmp;

    tmp = head->next;

	n = strlen(msg);
         	
    while(tmp != NULL)
   {
    //	sendto(connfd,msg,n,0,
	//			(struct sockaddr*)&cliaddr,sizeof(cliaddr));
	    send(tmp->connfd,msg,n,0);
        tmp = tmp->next;
   }
}

 void release_node(struct thread_node *connfd_node)
 {
	 Online * head;
     Online * p1 = NULL;
     Online * p2 = NULL;
	 int connfd;

	 head = connfd_node->head;
	 connfd = connfd_node->connfd;

     p1 = head->next;
	 p2 = head;

	 if(p1 == NULL)
	 {
		 printf("Link is empty!\n");
	 }
     else
     {
         while(p1->next != NULL && p1->connfd != connfd)
         {
             p2 = p1;
             p1 = p1 -> next;
         }

         if( p1 -> next != NULL)
         {
             p2 -> next = p1 -> next;
             free (p1);
		 }
		 else
		 {
			 if(p1->connfd == connfd)
			 {
				 p2->next = NULL;
				 free(p1);
			 }
			 else
			 {
				 printf("not node is attach!\n");
			 }
		 }
      }
}

void msg_send_recv(struct thread_node * connfd_node)
{
	    int n,i,m;
	//	char msg[1000];
        struct msg_node msg_text;

      //  n = recvfrom(connfd,msg,1000,0,
      //			    (struct sockaddr*)&cliaddr,&clilen);
      
		n = recv(connfd_node->connfd,&msg_text,sizeof(msg_text),0);

		if(0 == n)
		{
			printf("client offline!\n");

			release_node(connfd_node);

			pthread_exit(NULL);
		}
		else 
		{
	        m = strlen(msg_text.msg);

            printf("-----------------------\n");

            msg_text.msg[m] = '\0';
   
            printf("received the folloeing:\n");

            printf("%s\n",msg_text.msg);

            printf("-----------------------\n");

            for(i = 0; i < m;i++)
            {
        		msg_text.msg[i] = toupper(msg_text.msg[i]);
	        }
    	
	        if(strcmp(msg_text.chart_mode,"stoa") == 0)
	    	{
                visit_link_all(connfd_node->head,msg_text.msg);
	    	}
	    	else if(strcmp(msg_text.chart_mode,"stoo") == 0)
	    	{
                visit_link_one(connfd_node,&msg_text);
	    	}
		}
}

void * client_chart(void * arg)
{
    struct thread_node connfd_node;
//	char msg[1000];
    socklen_t clilen;
	struct sockaddr_in cliaddr;

	clilen = sizeof(cliaddr);

	connfd_node = *((struct thread_node*)arg);

    for(;;)
    {
        msg_send_recv(&connfd_node);
    }

    close(connfd_node.connfd);

	return NULL;
}

int main()
{
	int listenfd,connfd,n;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t clilen;
	char msg[1000];
	pid_t pid;
	int ret;
	pthread_t tid;
    Online * head;
	Online *new_node;

	struct thread_node connfd_node;

	create_link(&head);

	connfd_node.head = head;

	listenfd = socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family =  AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERVPORT);

	bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));

	listen(listenfd,100);

	while(1)
	{
    	connfd = accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);

        create_new_node(&new_node);

		printf("connfd = %d\n",connfd);

		new_node->connfd = connfd;
		connfd_node.connfd = connfd;

		insert_node_head(head,new_node);

        ret = pthread_create(&tid,NULL,(void *)client_chart,(void*)&connfd_node);
    }

	close(listenfd);

	release_link(&head);

    return 0;
}
