#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define SERVERPORT 8000

typedef struct chat_client
{
    struct sockaddr_in clientaddr;

    struct chat_client *next;
}Oline;

typedef struct msg_node
{
    char chat_mode[5];
    char msg[100];
}Msg;


void is_malloc_ok(Oline *new_node)
{
    if(new_node = NULL)
    {
        perror("malloc fail\n");
        exit(-1);
    }

}

void create_node(Oline **new_node)
{
    *new_node = (Oline *)malloc(sizeof(Oline));
    is_malloc_ok(*new_node);
}

void create_link(Oline **head)
{
    create_node(head);
    (*head)->next = NULL;
}

void insert_node_head(Oline *head,Oline *new_node)
{
    new_node->next = head->next;
    head->next = new_node;
}

void release_link(Oline **head)
{
    Oline *p = NULL;
    p = *head;

    while(*head != NULL)
    {
        *head = (*head)->next;
        free(p);
        p = *head;
    }
}

void delete_node(struct thread_node *conn_node)
{
    Oline *p1 = NULL;
    Oline *p2 = NULL;
    Oline *head = NULL;

    int conn_fd;

    head = conn_node->head;
    conn_fd = conn_node->conn_fd;

    p1= head->next;
    p2 = head;

    if(p1 = NULL)
    {
        printf("link id empty!\n");
    }

    else
    {
        while(p1 != NULL && p1->conn_fd != conn_fd)
        {
            p2 = p1;
            p1 = p1->next;
        }

        if(NULL == p1)
        {
            printf("NO SUCH CLIENT\n");
        }

        else
        {
            p2->next = p1->next;
            free(p1);
        }
    }
}



int main(int argc, char const *argv[])
{
    int server_fd;
    int n;
    socklen_t len;

    Oline *head;
    Oline *new_node;

    struct sockaddr_in serveraddr,clientaddr;

    Msg msg;

    create_link(&head);

    server_fd = socket(AF_INET,SOCK_DGRAM,0);

    bzero(&serveraddr,sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVERPORT);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

   // bind(server_fd,(struct sockaddr *)&serveraddr
   // ,sizeof(serveraddr));

    while(1)
    {
        len = sizeof(clientaddr);

        n = recvfrom(server_fd,&msg,sizeof(msg),0,(struct sockaddr *)&clientaddr,&len);

        create_node(&new_node);

        new_node->clientaddr = clientaddr;

        insert_node_head(head,new_node);

        buffer[n] = '\0';

        printf("接收到的消息是：");

        printf("%s",buffer);

        for(int i = 0; i < n;  i++)
        {
            buffer[i] = toupper(buffer[i]);
        }

        sendto(server_fd,buffer,n,0,(struct sockaddr *)&clientaddr,len);

        close(server_fd);
    }
    
    return 0;
}
