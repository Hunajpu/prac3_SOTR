SEMAPHORE *sem_create(int val)
{
	SEMAPHORE *mp = (SEMAPHORE *)malloc(sizeof(SEMAPHORE));
	mp->queue = 0;
	mp->value = val;
	return mp;
}

int P(SEMAPHORE *s)
{
	if(s->value > 0)
	{
		s->value--;
	}
	else
	{
		enqueue(&s->queue, running);
		printf("task %d blocked on sem=%p\n", running->pid, s);
		tswitch();
	}
}

int V(SEMAPHORE *s)
{
	if(s->value < 0)
	{
		return ERR_MUTEX_LOCK;
	}
	if(s->queue==0 || s->queue->pid != 0) //Si no hay hilos esperando
	{
		s->value++;
	}
	else //En caso de que haya hilos esperando
	{
		printf("%p: ",s);
		printList("s->queue", s->queue);
		PROC *p = dequeue(&s->queue);
		printf("task %d V up %d on sem=%p\n",running->pid, p->pid, s);
		enqueue(&readyQueue, p);
	}
}
