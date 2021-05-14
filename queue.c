/**********************queue.c file************************/
int enqueue(PROC **queue, PROC *p)
{
	PROC *q = *queue;
	if(q == 0 || p->priority > q->priority)
	{
		*queue = p;
		p->next = q;
	}
	else
	{
		while(q->next && p->priority <= q->next-> priority)
			q = q->next;
		p->next = q->next;
		q->next = p;
	}
}

PROC *dequeue(PROC **queue)
{
	PROC *p = *queue;
	if(p)
		*queue = (*queue)->next;
	return p;
}

int delFromQueue(PROC **queue, int event)
{
	PROC *q = *queue;
	
	while(q->next && q->next->event != event)
		q = q->next;
	if(q->next)
		q->next = q->next->next;
	else if(q->event == event)
	{
		q = NULL;
	}
}
int printList(char *name, PROC *p)
{
	printf("%s = ", name);
	while(p)
	{
		printf("%d --> ", p->pid);
		p = p->next;
	}
	printf("NULL\n");
}
