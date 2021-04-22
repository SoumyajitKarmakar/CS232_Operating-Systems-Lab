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

#include <sys/stat.h>

#include "tcp.h"
#include "request.h"

//The maximum number of requests that can be put in a queue at a time.
#define ALEN 10

//The mutex lock and the condition variable.
pthread_mutex_t* mut;
pthread_cond_t* cond;

//This is the fixed sized buffer to store all the requests. We will use this as a circular array.
static struct request* requests[ALEN] = { NULL };
//Two variables to store the index of the reading and writing end of the array.
int w = 0;
int r = 0;

void* workerThreads(void* arg) {
	//First we detach it so that we would not need to use the join function in the main section.
	pthread_detach(pthread_self());
	
	//We recieve the scheduling mode.
	int* m = (int*)arg;
	int mode = *m;

	//For FCFS we simply read the requests from the tail end one by one and execute them.
	if (mode == 1) {
		while (1) {
			pthread_mutex_lock(mut);
			//Wait if the buffer is empty.
			while (requests[r] == NULL)
				pthread_cond_wait(cond, mut);

			struct request* req = requests[r];
			//After taking out the request we clean its location in the buffer.
			requests[r] = NULL;
			//Increment the pointer in a circular fashion.
			r = (r + 1) % ALEN;

			//Signal the writer, if it was waiting and release the lock.
			pthread_cond_signal(cond);
			pthread_mutex_unlock(mut);

			//Handle the request.
			printf("webserver: got request for %s\n", req->filename);
			request_handle(req);

			printf("webserver: done sending %s\n", req->filename);
			request_delete(req);
		}
	}
	//For SFF everytime we perform a sort operation according to the file size and then pick the smallest one.
	else if (mode == 2) {
		struct stat st1, st2;
		off_t s1, s2;
		struct request* treq;
		while (1) {
			pthread_mutex_lock(mut);
			while (requests[r] == NULL)
				pthread_cond_wait(cond, mut);

			//Sort all the existing requests.
			int i, j;
			for (i = r; i < w - 1; i++) {
				for (j = r; j < w - i -1; j++) {
					stat(requests[j]->filename, &st1);
					stat(requests[j + 1]->filename, &st2);
					s1 = st1.st_size;
					s2 = st2.st_size;
					if (s1 > s2) {
						//Swap.
						treq = request[j];
						requests[j] = requests[j + 1];
						requests[j + 1] = treq;
					}
				}
			}

			//Get the request with smallest size.
			struct request* req = requests[r];
			requests[r] = NULL;
			r = (r + 1) % ALEN;

			pthread_cond_signal(cond);
			pthread_mutex_unlock(mut);

			//Handle the request.
			printf("webserver: got request for %s\n", req->filename);
			request_handle(req);

			printf("webserver: done sending %s\n", req->filename);
			request_delete(req);
		}
	}

	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	if (argc < 4) {
		fprintf(stderr, "use: %s <port> <no of threads> <scheduling mode, FCFS or SFF>\n", argv[0]);
		return 1;
	}

	if (chdir("webdocs") != 0) {
		fprintf(stderr, "couldn't change to webdocs directory: %s\n", strerror(errno));
		return 1;
	}

	int port = atoi(argv[1]);
	int nthreads = atoi(argv[2]);
	int mode = 0;

	//We check if the user made any mistake or not.
	if(strcmp(argv[3], "FCFS") == 0)
		mode = 1;
	else if (strcmp(argv[3], "SFF") == 0)
		mode = 2;
	else {
		fprintf(stderr, "Please enter a valid scheduling mode, ie., FCFS or SFF.\n");
		return 1;
	}

	struct tcp* master = tcp_listen(port);
	if (!master) {
		fprintf(stderr, "couldn't listen on port %d: %s\n", port, strerror(errno));
		return 1;
	}

	printf("webserver: waiting for requests..\n");

	//Allocate memory and initialise the mutex lock and condition variable.
	mut = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if (mut == NULL) {
		return (NULL);
	}

	cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
	if (cond == NULL) {
		free(mut);
		return (NULL);
	}

	pthread_mutex_init(mut, NULL);
	pthread_cond_init(cond, NULL);

	//Create and initialise the worker threads.
	pthread_t p[nthreads];
	int i = 0;

	for (i = 0; i < nthreads; i++) {
		pthread_create(&p[i], NULL, workerThreads, &mode);
	}

	while (1) {
		struct tcp* conn = tcp_accept(master, time(0) + 300);
		if (conn) {

			printf("webserver: got new connection.\n");
			struct request* req = request_create(conn);
			
			if (req) {
				//Get the lock.
				pthread_mutex_lock(mut);
				//Wait if buffer already full.
				while (requests[w] != NULL)
					pthread_cond_wait(cond, mut);

				//Add request if not full.
				requests[w] = req;
				//Increment the index in circular manner.
				w = (w + 1) % ALEN;

				//Wake up the worker threads if asleep.
				pthread_cond_broadcast(cond);
				//Release the lock.
				pthread_mutex_unlock(mut);
			}
			else {
				tcp_close(conn);
			}
		}
		else {
			printf("webserver: shutting down because idle too long\n");
			break;
		}
	}

	return 0;
}