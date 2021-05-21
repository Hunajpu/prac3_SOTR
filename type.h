/****************type.h file********************/
#define NPROC		9		//Number of PROCs
#define SSIZE		1024	//stack size = 1kB

//PROC status
#define FREE   0
#define READY  1
#define SLEEP  2
#define BLOCK  3
#define ZOMBIE 4

typedef struct proc
{
	struct proc *next;
	int ksp;
	int pid;
	
	int priority;
	int status;
	int event;
	int exitStatus;
	int joinPid;
	struct proc *joinPtr;
	int stack[SSIZE];
}PROC;

typedef struct mutex{
	int lock; // Estado de desbloqueo = 0. Bloloqueo = 1;
	PROC *owner; // Apuntador al dueño del mutex
	PROC *queue; // FIFO queue of BLOCKED waiting PROCs
}MUTEX;
