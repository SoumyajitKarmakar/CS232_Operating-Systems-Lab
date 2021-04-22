#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

//This is the total number of vvalues the writer function will enter.
#define MAXCOUNT 5
//These are the time in microsecond for which each thread will sleep in between their various statemest so that the other threads have the chance to execute in between.
#define WRITER1 1000000
#define READER1 1000000
#define READER2 1000000
#define READER3 1000000
#define READER4 1000000

//First of all we define a data structure for the proper functioning of lock.
typedef struct {
	//The mutex lock variable, which acts as the lock.
	pthread_mutex_t *mut;
	//Number of writers ready to write.
	int writers;
	//Number of readers ready to read.
	int readers;
	//Number of threads waiting for their turn.
	int waiting;
	//The condition variables which determine the open and close of locks.
	pthread_cond_t *writeOK, *readOK;
} rwl;

//We will get to know what each of these functions do when we reach their bodies.
rwl *initlock (void);
void readlock (rwl *lock, int d);
void writelock (rwl *lock, int d);
void readunlock (rwl *lock);
void writeunlock (rwl *lock);
void deletelock (rwl *lock);

//A data structure which stores the process id of the reader and writer processes, the time for which each process sleeps, and the lock. Variables of this datatype is passed as argument when the thread functions are called.
typedef struct {
	rwl *lock;
	int id;
	long delay;
} rwargs;

rwargs *newRWargs (rwl *l, int i, long d);
void *reader (void *args);
void *writer (void *args);

//The global data variable which is updated by the writer and read by the readers.
static int data = 1;

int main (){
	//First we assign 5 thereads for each process, 1 writer and 4 readers.
	pthread_t r1, r2, r3, r4, w1;
	//Now we create the variables which contain information about the processes and the lock which are passed as arguments.
	rwargs *a1, *a2, *a3, *a4, *a5;
	//We create and initialise the lock variable.
	rwl *lock;
	lock = initlock ();

	//In the following lines we initialise each process variable and start a thread for each of the processes. We pass the process variable as arguments to the reader and writer functions.
	a1 = newRWargs (lock, 1, WRITER1);
	pthread_create (&w1, NULL, writer, a1);

	a2 = newRWargs (lock, 1, READER1);
	pthread_create (&r1, NULL, reader, a2);

	a3 = newRWargs (lock, 2, READER2);
	pthread_create (&r2, NULL, reader, a3);

	a4 = newRWargs (lock, 3, READER3);
	pthread_create (&r3, NULL, reader, a4);

	a5 = newRWargs (lock, 4, READER4);
	pthread_create (&r4, NULL, reader, a5);

	//Here we wait for the completion of all the processes.
	pthread_join (w1, NULL);
	pthread_join (r1, NULL);
	pthread_join (r2, NULL);
	pthread_join (r3, NULL);
	pthread_join (r4, NULL);

	//We free up the memory since we no longer need anything.
	free (a1);
	free (a2);
	free (a3);
	free (a4);
	free (a5);

	return 0;
}

//This function initialises the process variable.
rwargs *newRWargs (rwl *l, int i, long d)
{
	rwargs *args;

	//Allocate space for the variable.
	args = (rwargs *)malloc (sizeof (rwargs));
	
	//If unsuccessful return nothing.
	if (args == NULL)
		return (NULL);

	//Assign values.
	args->lock = l;
	args->id = i;
	args->delay = d;

	return (args);
}

//This function is executed by the reader threads to read the value.
void *reader (void *args)
{
	rwargs *a;
	int d;

	a = (rwargs *)args;

	do {
		//First of all the reader invokes the funtion to lock the mutex.
		readlock (a->lock, a->id);
		//Reads the data.
		d = data;
		//Sleeps.
		usleep (a->delay);
		//Invokes the funtion to unlock the lock.
		readunlock (a->lock);
		printf ("Reader %d : Data = %d\n", a->id, d);
		usleep (a->delay);
		//The loop exits when the writers has finished writing and no more values will be entered.
	} while (d != 0);
	printf ("Reader %d: Finished.\n", a->id);

	return (NULL);
}

//This is the writer function to change the data value.
void *writer (void *args)
{
	rwargs *a;
	int i;

	a = (rwargs *)args;

	//This loop will run MAXCOUNT number of times.
	for (i = 2; i < MAXCOUNT; i++) {
		//It invokes the function to aquire the lock.
		writelock (a->lock, a->id);
		//Changes the data.
		data = i;
		//Sleeps.
		usleep (a->delay);
		//Releases the lock.
		writeunlock (a->lock);
		printf ("Writer %d: Wrote %d\n", a->id, i);
		usleep (a->delay);
	}
	printf ("Writer %d: Finishing...\n", a->id);
	//Gets the lock for one final entry.
	writelock (a->lock, a->id);
	//Enters the terminating value.
	data = 0;
	//Invokes the function to release the mutex lock.
	writeunlock (a->lock);
	printf ("Writer %d: Finished.\n", a->id);
	
	return (NULL);
}

//This function initialises the mutex lock vaiable.
rwl *initlock (void){
 	rwl *lock;
	//Allocate memory for the data structure.
 	lock = (rwl *)malloc (sizeof (rwl));

	//Return if unseccessfull.
 	if (lock == NULL)
		return (NULL);

	//Allocate memory for the mutex variable.
 	lock->mut = (pthread_mutex_t *) malloc (sizeof(pthread_mutex_t));

	//Free the previously allocated memory and return if unsucessful.
 	if (lock->mut == NULL){
		free (lock);
		return (NULL);
	}

	//Allocate memory for the writer condition variable.
 	lock->writeOK = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));

	//Free the previously allocated memory and return if unsucessful.
 	if (lock->writeOK == NULL){
		free (lock->mut);
		free (lock);
	   	return (NULL);
	}

	//Allocate memory for the reader condition variable.
	lock->readOK = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));

	//Free the previously allocated memory and return if unsucessful.
	if (lock->writeOK == NULL){
		free (lock->mut);
		free (lock->writeOK);
	   	free (lock);
		return (NULL);
	}

	//Initialise the variables with default values.
	pthread_mutex_init (lock->mut, NULL);
	pthread_cond_init (lock->writeOK, NULL);
	pthread_cond_init (lock->readOK, NULL);

	lock->readers = 0;
	lock->writers = 0;
	lock->waiting = 0;

	return (lock);
}

//This function is called before the reader thread reads the value.
void readlock (rwl *lock, int d)
{
	//It aquires the lock.
	pthread_mutex_lock (lock->mut);
	//If there are no writer writing or no thread waiting then run this loop.
	if (lock->writers || lock->waiting) {
		do {
			printf ("reader %d blocked.\n", d);
			//Here the thread will release the lock and wait on the condition variavle, ie. the readOK variable.
			pthread_cond_wait (lock->readOK, lock->mut);
			printf ("reader %d unblocked.\n", d);
		} while (lock->writers);
	}

	//Increments the variable which stores the number of readers currently reading the data.
	lock->readers++;
	//Releases the lock.
	pthread_mutex_unlock (lock->mut);

	return;
}

//This function is invoked before the writer starts to edit the value.
void writelock (rwl *lock, int d)
{
	//Firstly it aquires the lock.
	pthread_mutex_lock (lock->mut);
	//Increments the number of threads waiting.
	lock->waiting++;

	//If there are are no readers or writers available then this loop is executed.
	while (lock->readers || lock->writers) {
		printf ("writer %d blocked.\n", d);
		//Here the thread will release the lock and wait on the condition variable, ie. the writeOK variable.
		pthread_cond_wait (lock->writeOK, lock->mut);
		printf ("writer %d unblocked.\n", d);
	}

	//Now it will decrement the number of processes waiting 
	lock->waiting--;
	//Now the writer can do its intended job and so the number of writers currently working is incremented.
	lock->writers++;
	//The lock is released.
	pthread_mutex_unlock (lock->mut);

	return;
}

//This function is called after each reader is done reading the value.
void readunlock (rwl *lock)
{
	//It aquires the lock.
	pthread_mutex_lock (lock->mut);
	//Decrements the variable which stores the number of readers currently reading.
	lock->readers--;
	//It sends a signal to the writer thread.
	pthread_cond_signal (lock->writeOK);
	//It realeses the lock.
	pthread_mutex_unlock (lock->mut);
}


//This function is called after the writer is done editing the global data variable.
void writeunlock (rwl *lock)
{
	//It invokes the function to aquire the lock.
	pthread_mutex_lock (lock->mut);
	//Decrements the variable which stores the number of writers currently writing.
	lock->writers--;

	//This function is used to signal all the other threads, ie. all the other reader threads. Instead of using the broadcast function we can simply use 4 signal function one by one to signal all the readers individually.
	//pthread_cond_broadcast (lock->readOK);
	pthread_cond_signal(lock->readOK);
	pthread_cond_signal(lock->readOK);
	pthread_cond_signal(lock->readOK);
	pthread_cond_signal(lock->readOK);

	//We release the mutex lock.
	pthread_mutex_unlock (lock->mut);
}

//Before exiting the program this funtion frees up the memory so that it can be used for other purposes.
void deletelock (rwl *lock)
{
	pthread_mutex_destroy (lock->mut);
	pthread_cond_destroy (lock->readOK);
	pthread_cond_destroy (lock->writeOK);
	free (lock);

	return;
}

