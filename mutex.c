/*******************mutex.c file*********************/

MUTEX *mutex_create()
{
	MUTEX *mp = (MUTEX *)malloc(sizeof(MUTEX));
	mp->lock = mp->owner = mp->queue = 0;
	return mp;
}

void mutex_destroy(MUTEX *mp){free(mp);}

int mutex_lock(MUTEX *mp)
{
	if(mp->lock == 0)
	{
		printf("task %d mutex_lock() ", running->pid);
		mp->lock = 1;
		mp->owner = running;
	}
	else
	{
		enqueue(&mp->queue, running);
		tswitch();
	}
}

int mutex_unlock(MUTEX *mp)
{
	if(mp->lock == 0 || (mp->lock && running != mp->owner))
	{
		return ERR_MUTEX_LOCK;
	}
	// El mutex esta bloqueado y el llamado lo hace el propietario
	if(mp->queue == 0)
	{
		printf(" task %d mutex_unlock()\n", mp->owner->pid);
		mp->lock = 0;
		mp->owner = 0;
	}
	else //En caso de que haya elementos esperando
	{
		PROC *p = dequeue(&mp->queue);
		mp->owner = p;
		enqueue(&readyQueue, p);
	}
}
