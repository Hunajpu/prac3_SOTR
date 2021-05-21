/*****************t.c USER-level Threads*********************/
/*
 * REF: Systems Programing in Unix/Linux, K.C Wang, editorial
 * Springer Nature, 2018. Chapter 4 Concurrent Programing, page 166.
 * Programing Project: User-Level Threads
 * 
 * To compile and run the base code under Linux, enter
 * 	gcc -m32 t3.c ts.s
 * Then run a.out. While running the program, the reader may enter the
 * comands
 * 'c': create new task
 * 's': switch to run the next task from readyQueue
 * 'q': let the running task terminate
 * to test and observe task execution in the system
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "type.h"

#define ERR_NOPID 5
#define ERR_DEADLOCK 6
#define ERR_MUTEX_LOCK 7
#define N 4

PROC proc[NPROC];
PROC *freeList;
PROC *readyQueue;
PROC *sleepList;
PROC *running;

void tswitch();
#include "queue.c"
#include "wait.c"
#include "mutex.c"

int A[N][N]; //Matriz A
MUTEX *mp;   // mutex de sincronización
int total;	 // valor final
int create(void (*f)(), void *parm);
void func(void *parm);


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
	
	printf("P0: initialize A matrix\n");
	for(i=0; i<N;i++)
	{
		for(j=0; j<N; j++)
		{
			A[i][j] = i*N + j + 1;
		}
	}
	
	// Imprimir la matriz
	for(i=0; i<N; i++)
	{
		for(j=0; j<N; j++)
		{
			printf("%4d ", A[i][j]);
		}
		printf("\n");
	}
	
	mp = mutex_create();
	total = 0;
}

int myexit()
{
	texit(0);
}

void task1(void *parm)
{
	int pid[N];
	int i, status;
	int me = running->pid;
	printf("task %d: create working tasks : ", me);
	for(i=0; i<N; i++)
	{
		pid[i]=create(func, (void*)i);
		printf("%d ", pid[i]);
	}
	printf(" to compute matrix row sums\n");
	for(i=0; i<N; i++)
	{
		printf("task %d tries to join with task %d\n", running->pid, pid[i]);
		join(pid[i], &status);
	}
	
	printf("task %d : total = %d\n", me, total);
}

void func(void *parm)
{
	int i, row, s;
	int me = running->pid;
	row = (int)parm;
	printf("task %d computes sum of row %d\n", me, row);
	s = 0;
	for(i=0; i<N; i++)
	{
		s += A[row][i];
	}
	printf("task %d update total with %d\n", me, s);
	mutex_lock(mp);
	total += s;
	printf("[total = %d] ", total);
	mutex_unlock(mp);
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
