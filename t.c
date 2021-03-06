/*****************t.c USER-level Threads*********************/
/*
 * REF: Systems Programing in Unix/Linux, K.C Wang, editorial
 * Springer Nature, 2018. Chapter 4 Concurrent Programing, page 166.
 * Programing Project: User-Level Threads
 * 
 * To compile and run the base code under Linux, enter
 * 	gcc -m32 t.c ts.s
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

PROC proc[NPROC];
PROC *freeList;
PROC *readyQueue;
PROC *sleepList;
PROC *running;


int do_exit();
void tswitch();
#include "queue.c"
#include "wait.c"
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
		p->joinPid = 0;
		p->joinPtr = 0;
		p->next = p+1;
	}
	proc[NPROC-1].next = 0;
	
	freeList = &proc[0];
	readyQueue = 0;
	sleepList = 0;
	// Crear P0
	running = p = dequeue(&freeList);
	p->status=READY;
	p->priority = 0;
	printList("freeList", freeList);
	printf("init complete\n");
}

int do_create()
{
	int pid = create(func, (void *)(running->pid));
	return pid;
}

int do_switch()
{
	tswitch();
}

int do_exit()
{
	texit(running->pid);
}

int do_join()
{
	int status, ic;
	char c;
	
	printf("enter a pid to join with : ");
	c = getchar(); getchar();
	ic = c - '0';
	join(ic, &status);
}

void task1(void *parm)
{
	int pid[2];
	int i, status;
	printf("task 1 running\n");
	for(i=0; i<2; i++)
	{
		pid[i]=create(func, (void*)running->pid);
	}
	join(5, &status);
	for(i=0; i<2; i++)
	{
		pid[i] = join(pid[i], &status);
		printf("task%d joined with task%d: status = %d\n", running->pid, pid[i], status);
	}
}

void func(void *parm)
{
	int c;
	//printf("task%d start: parm = %d\n", running->pid, parm);
	while(1)
	{
		printList("readyQueue", readyQueue);
		printf("task%d running: ", running->pid);
		printf("enter a key [c|s|q|j] : ");
		
		c = getchar(); getchar();
		switch(c)
		{
			case 'c' : do_create(); break;
			case 's' : do_switch(); break;
			case 'q' : do_exit(); break;
			case 'j' : do_join(); break;
		}
	}
}

int create(void (*f)(), void *parm)
{
	int i;
	PROC *p = dequeue(&freeList);
	if(!p)
	{
		printf("create failed\n");
		return -1;
	}
	p->status = READY;
	p->priority = 1;
	p->joinPid = 0;
	p->joinPtr = 0;
	
	for(i=1; i<13; i++)
		p->stack[SSIZE-i] = 0;
	p->stack[SSIZE-1]= (int)parm;
	p->stack[SSIZE-2]= (int)do_exit;
	p->stack[SSIZE-3]= (int)f;
	p->ksp = (int)&p->stack[SSIZE-12];
	enqueue(&readyQueue, p);
	//printList("readyQueue", readyQueue);
	printf("task %d created a new task %d\n", running->pid, p->pid);
	return p->pid;
}

int main()
{
	int i, pid, status;
	printf("Welcome to the MT User-Level Threads System\n");
	init();
	create((void *)task1,0);
	printf("P0 switch to P1 \n");
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
