#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"
#include "semaphore.h"

struct queue q;
struct thread_mutex mx;// a mutex object lock 
struct sem_t empty;// a semaphore object empty
struct sem_t full;// a semaphore object full
struct thread_mutex printLock;

void init_semaphore()
{
	thread_mutex_init(&mx);// initialize mutex lock
	sem_init(&empty,5);// initialize semaphore empty with 5
	sem_init(&full,0);// initialize semaphore full with 0

}

void  ProducerFunc(void * arg)
{	
	thread_mutex_lock(&printLock);
	printf("%s\n",(char*)arg);
	thread_mutex_unlock(&printLock);
	int i;
	for(i=1;i<=10;i++)
	{
		// wait for semphore empty
		sem_wait(&empty);
		// wait for mutex lock
		thread_mutex_lock(&mx);
		push(&q,i);
		thread_mutex_lock(&printLock);
		printf("producer produced item %d\n",i);
		thread_mutex_unlock(&printLock);
		// unlock mutex lock
		thread_mutex_unlock(&mx);	
		sleep(1);
		// post semaphore full
		sem_post(&full);
	}
	exit_thread() ;
}

void  ConsumerFunc(void * arg)
{
	thread_mutex_lock(&printLock);
	printf("%s\n",(char*)arg);
	thread_mutex_unlock(&printLock);
	int i;
	for(i=1;i<=10;i++)
	{	
		// wait for semphore full
		sem_wait(&full);
		// wait for mutex lock
 		thread_mutex_lock(&mx);
		int item = front(&q);
		pop(&q);
		thread_mutex_lock(&printLock);
		printf("consumer consumed item %d\n",item);	
		thread_mutex_unlock(&printLock);

		// unlock mutex lock
		thread_mutex_unlock(&mx);
		sleep(1);
		// post semaphore empty	
		sem_post(&empty);	
	}
	exit_thread() ;
}

int main(void)
{	
	
	init_semaphore();
	thread_mutex_init(&printLock);
    init_queue(&q) ;
	char * message1 = "i am producer";
	char * message2 = "i am consumer";


	void *s1, *s2;
  	int thread1, thread2;

  	s1 = malloc(4096);
  	s2 = malloc(4096);

  	thread1 = create_thread(ProducerFunc, (void*)message1, s1);
	// sleep(1);
  	thread2 = create_thread(ConsumerFunc, (void*)message2, s2); 

  	join_thread(thread1);
  	join_thread(thread2);	
	
	exit(0);
}
//producer_consumer