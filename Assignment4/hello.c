#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

void print_message_function( void *ptr );

void main() {
	pthread_t thread1, thread2;

	char *message1 = "Hello";
	char *message2 = "World";

	pthread_create(&thread1, NULL, (void *) &print_message_function, (void *) message1);
	pthread_create(&thread2, NULL, (void *) &print_message_function, (void *) message2);
	
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	exit(0);
}
void print_message_function( void *ptr ) {
	char *message;
	message = (char *) ptr;
	printf("%s\n", message);
	pthread_exit(0);
}

