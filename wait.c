/***************file wait.c********************/

int tsleep(int event)
{
	running->event = event;
	running->status = SLEEP;
	enqueue(&sleepList, running);
	printList("sleepList", sleepList);
	tswitch();
}

int twakeup(int event)
{
	PROC *p = sleepList;
	while(p)
	{
		if(p->event == event)
		{
			//dequeue(&sleepList);
			delFromQueue(&sleepList, event);
			p->status == READY;
			enqueue(&readyQueue, p);
		}
		p = p->next;
	}
	//printList("readyQueue", readyQueue);
}

int texit(int status)
{
	PROC *sq = sleepList;
	int isFree = 1;
	// Buscar tareas que se quieran unir a esta
	while(sq)
	{
		if(sq->joinPid == running->pid)
			isFree = 0;
		sq = sq->next;
	}
	if(isFree)
	{
		running->status = FREE;
		running->priority = 0;
		enqueue(&freeList,running);
		//printf("task%d: no joiner=>exit as FREE: ", running->pid);
	}
	else // Alguna tarea esta esperando que esta termine
	{
		running->exitStatus = status;
		running->status = ZOMBIE;
		twakeup(running->pid);
	}
	//printf("task%d exited with status = %d\n", running->pid, status);
	tswitch();
}

int join(int targetPid, int *status)
{
	PROC *sq = readyQueue;
	PROC *p;
	int noPID = 1;
	int deadErr = 0;
	while(1)
	{
		// Buscar tarea en readyQueue
		while(sq)
		{
			if(sq->pid == targetPid)
			{
				noPID = 0;
				p = sq;
			}
			sq = sq->next;
		}
		// Buscar tarea en sleepList
		sq = sleepList;
		while(sq)
		{
			if(sq->pid == targetPid)
			{
				noPID = 0;
				p = sq;
			}
			sq = sq->next;
		}
		if(noPID)
		{
			//printf("task%d try to join with task%d. No pid = %d\n", running->pid, targetPid,targetPid);
			return ERR_NOPID;
		}
	
		// Checar si el join pointer de la tarea targetPid no apunta a
		// esta tarea
		sq = p;
		if(sq)
		{
			sq = sq->joinPtr;
			while(sq)
			{
				if(sq == running)
				{
					deadErr = 1;
				}
				sq = sq->next;
			}
		}
		
		if(deadErr)
		{
			printf("task%d try to join with task%d. Deadlock error\n", running->pid, targetPid);
			return ERR_DEADLOCK;
		}
		
		running->joinPid = targetPid;
		running->joinPtr = p;
		if(p->status == ZOMBIE)
		{
			*status = p->exitStatus;
			p->status = FREE;
			p->priority = 0;
			enqueue(&freeList, p);
			return p->pid;
		}
		//printf("task%d try to join with task%d: ", running->pid, p->pid);
		tsleep(targetPid);
	}
}
