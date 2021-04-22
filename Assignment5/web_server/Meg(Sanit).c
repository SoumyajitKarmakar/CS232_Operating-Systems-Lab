#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "tcp.h"
#include "request.h"

struct node
{
    struct request* data;
    struct node* next;
};
struct node* head;
int size = 0;

void enqueue(struct request* d)
{
    struct node* ptr, * temp;
    ptr = (struct node*)malloc(sizeof(struct node));
    if (ptr == NULL)
    {
        printf("\nOVERFLOW");
    }
    else
    {
        size++;
        ptr->data = d;
        if (head == NULL)
        {
            ptr->next = NULL;
            head = ptr;
        }
        else
        {
            temp = head;
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = ptr;
            ptr->next = NULL;
        }
    }
}
struct request* remove_fcfs()
{
    struct node* ptr;
    if (head == NULL)
    {
        return NULL;
    }
    else
    {
        size--;
        ptr = head;
        head = ptr->next;
        return ptr->data;
    }
}

struct request* remove_sff()
{
    if (head == NULL)
    {
        return NULL;
    }
    else
    {
        int found = 0;
        struct node* temp = head;
        while (temp->next != NULL && temp->next->next != NULL)
        {
            while (strstr(temp->next->data->filename, "thumb") != NULL)
            {
                found = 1;
                size--;
                struct node* ptr;
                ptr = temp->next;
                temp->next = ptr->next;
                return ptr->data;
            }
        }
        if (found == 0)
            return remove_fcfs();
    }
    return NULL;
}



pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int me;

void* worker()
{
    while (1)
    {
        if (size > 0)
        {
            pthread_mutex_lock(&mutex);
            struct request* req;
            if (me == 1)
                req = remove_fcfs();
            else
                req = remove_sff();
            pthread_mutex_unlock(&mutex);
            if (req != NULL)
            {
                printf("webserver: got request for %s\n", req->filename);
                request_handle(req);
                printf("webserver: done sending %s\n", req->filename);
                request_delete(req);
            }
        }
    }
    return NULL;
}
int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "use: %s <port>\n", argv[0]);
        return 1;
    }

    if (chdir("webdocs") != 0) {
        fprintf(stderr, "couldn't change to webdocs directory: %s\n", strerror(errno));
        return 1;
    }

    int port = atoi(argv[1]);

    struct tcp* master = tcp_listen(port);
    if (!master) {
        fprintf(stderr, "couldn't listen on port %d: %s\n", port, strerror(errno));
        return 1;
    }

    printf("webserver: waiting for requests..\n");

    if (!strcmp(argv[3], "FCFS"))
        me = 1;
    else
        me = 2;
    pthread_t t[atoi(argv[2])];
    for (int i = 0; i < atoi(argv[2]); ++i)
    {
        pthread_create(&t[i], NULL, worker, NULL);

    }


    while (1) {
        struct tcp* conn = tcp_accept(master, time(0) + 300);
        if (conn) {
            printf("webserver: got new connection.\n");
            struct request* req = request_create(conn);

            if (req)
            {
                pthread_mutex_lock(&mutex);
                if (size <= 10)
                    enqueue(req);
                else
                    printf("server full!!!\n");
                pthread_mutex_unlock(&mutex);
            }
            else
                tcp_close(conn);

        }
        else {
            printf("webserver: shutting down because idle too long\n");
            break;
        }
    }

    return 0;
}