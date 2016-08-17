#include "comm.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

task_t *task_init()
{
	task_t *t = calloc(1, sizeof(task_t));
	return t;
}

int task_push(task_t *t, void *v)
{
	if ((t->tail + 1) % TASKSIZE == t->head)
		return ETASKFULL;
	t->task[t->tail] = v;
	t->tail = (t->tail + 1) % TASKSIZE;
	return 0;
}

int task_pop(task_t *t, void **v)
{
	if (t->tail == t->head)
		return ETASKEMPTY;
	*v = t->task[t->head];
	t->head = (t->head + 1) % TASKSIZE;
	return 0;
}

int task_count(task_t *t)
{
	return (t->tail - t->head + TASKSIZE) % TASKSIZE;
}

void task_free(task_t *t)
{
	free(t);
}


int set_fd_nonblock(int fd)
{
	int fl = fcntl(fd, F_GETFL);
	fl |= O_NONBLOCK;
	return fcntl(fd, F_SETFL, fl);
}

void print_head(head_t *t)
{
	
}
