#ifndef _COMM_H_
#define _COMM_H_

#define TASKSIZE    1024

#define ETASKFULL   -1
#define ETASKEMPTY  -2

#define FD_IN   1
#define FD_ERR  2

#define MAGICNUM    198902


typedef struct task_s {
	int head;
	int tail;
	void * volatile task[TASKSIZE];
} task_t;

task_t *task_init();
extern int task_push(task_t *t, void *v);
extern int task_pop(task_t *t, void **v);
extern int task_count(task_t *t);
extern void task_free(task_t *t);

extern int set_fd_nonblock(int fd);

typedef struct head_s {
	int magic_num;
	int cmd;
	int content_len;
} head_t;

void print_head(head_t *h);


#endif
