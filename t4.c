/*
 * 
 * 
 * Programa de pruebas para los semaforos
 *
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include "type.h"

#define ERR_NOPID 5
#define ERR_DEADLOCK 6
#define ERR_MUTEX_LOCK 7
PROC proc[NPROC], *freeList, *sleepList, *readyQueue, *running;
void tswitch();
#include "queue.c"
#include "wait.c"
#include "semaphore.c"

#define NBUF 4
#define N	 8
int buf[NBUF], head, tail;

int create(void (*f)(), void *parm);

SEMAPHORE *full, *empty, *mutx;

void producer()
{
	int i;
	printf("producer %d start\n", running->pid);
	for(i=0; i<N; i++)
	{
		P(empty);
		P(mutx);
		buf[head++] = i+1;
		printf("producer %d: item = %d\n", running->pid, i+1);
		head %= NBUF;
		V(mutx);
		V(full);
	}
	printf("producer %d exit\n", running->pid);
}
void consumer()
{
	int i, c;
	printf("consumer %d start\n",running->pid);
	for(i=0; i<N; i++)
	{
		P(full);
		P(mutx);
		c=buf[tail++];
		tail %= NBUF;
		printf("consumer %d: got item = %d\n", running->pid, c);
		V(mutx);
		V(empty);
	}
}

int init()
{
	int i,j;
	PROC *p;
	for(i=0; i<NPROC; i++)
	{
		p = &proc[i];
		p->pid = i;
		p->priority = 0;
		p->status = FREE;
		p->event = 0;
		p->next = p+1;
	}
	proc[NPROC-1].next = 0;
	
	freeList = &proc[0];
	readyQueue = 0;
	sleepList = 0;
	running = p = dequeue(&freeList);
	p->status = READY;
	p->priority = 0;
	printList("freeList", freeList);
	
	// Inicializar los semaforos
	head = tail = 0;
	//full.value = 0;
	//full.queue = 0;
	//empty.value = NBUF;
	//empty.queue = 0;
	//mutx.value = 1;
	//mutx.queue = 0;
	full = sem_create(0);
	empty = sem_create(NBUF);
	mutx = sem_create(1);
	
	printf("init complete\n");
}

int myexit()
{
	texit(0);
}

int task1()
{
	int status;
	printf("task %d creates producer-consumer tasks\n", running->pid);
	create((void *)producer, 0);
	create((void *)consumer, 0);
	join(2,&status);
	join(3,&status);
	printf("task %d exit\n", running->pid);
}

int create(void (*f)(), void *parm)
{
	int i;
	PROC *p = dequeue(&freeList);
	if(!p)
	{
		printf("fork failed\n");
		return -1;
	}
	p->status = READY;
	p->priority = 1;
	p->joinPid = 0;
	p->joinPtr = 0;
	
	for(i=1; i<13; i++)
		p->stack[SSIZE-i] = 0;
	p->stack[SSIZE-1]= (int)parm;
	p->stack[SSIZE-2]= (int)myexit;
	p->stack[SSIZE-3]= (int)f;
	p->ksp = (int)&p->stack[SSIZE-12];
	enqueue(&readyQueue, p);
	//printList("readyQueue", readyQueue);
	//printf("task %d created a new task %d\n", running->pid, p->pid);
	return p->pid;
}

int main()
{
	int i, pid, status;
	printf("Welcome to the MT User-Level Threads System\n");
	init();
	create((void *)task1,0);
	tswitch();
	printf("All tasks ended: P0 loops\n");
	while(1);
}

int scheduler()
{
	if(running->status == READY)
		enqueue(&readyQueue, running);
	running = dequeue(&readyQueue);
	//printf("next running = %d\n", running->pid);
}
