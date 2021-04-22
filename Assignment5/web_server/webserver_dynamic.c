
/*
This is the main program for the simple webserver.
Your job is to modify it to take additional command line
arguments to specify the number of threads and scheduling mode,
and then, using a pthread monitor, add support for a thread pool.
All of your changes should be made to this file, with the possible
exception that you may add items to struct request in request.h
*/

#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include "tcp.h"
#include "request.h"

#define MAXTHREADS 10

void* func(void* arg) {
	pthread_detach(pthread_self());

	struct request* req = (struct request*)arg;

	printf("webserver: got request for %s\n", req->filename);
	request_handle(req);

	printf("webserver: done sending %s\n", req->filename);
	request_delete(req);

	pthread_exit(NULL);
}

int main( int argc, char *argv[] )
{
	if(argc<2) {
		fprintf(stderr,"use: %s <port>\n",argv[0]);
		return 1;
	}

	if(chdir("webdocs")!=0) {
		fprintf(stderr,"couldn't change to webdocs directory: %s\n",strerror(errno));
		return 1;
	}

	int port = atoi(argv[1]);

	struct tcp *master = tcp_listen(port);
	if(!master) {
		fprintf(stderr,"couldn't listen on port %d: %s\n",port,strerror(errno));
		return 1;
	}

	printf("webserver: waiting for requests..\n");

	pthread_t p[MAXTHREADS];
	int i = 0;

	while(1) {
		struct tcp* conn = tcp_accept(master,time(0)+300);
		if(conn) {
			
			printf("webserver: got new connection.\n");
			struct request* req = request_create(conn);
			if (req) {
				
				pthread_create(&p[i++], NULL, func, (void *)req);
				//pthread_join(p, NULL);
			}
			else {
				tcp_close(conn);
			}
		} else {
			printf("webserver: shutting down because idle too long\n");
			break;
		}
	}

	return 0;
}




