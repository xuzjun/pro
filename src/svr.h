#ifndef _SVR_H_
#define _SVR_H_

#include <pthread.h>
#include "comm.h"

#define PORT       9999
#define MAXLISTEN  1024
#define MAXEVENTS  1024

struct svr {
	short port;
	int   listenfd;

	task_t *rin;
	task_t *rout;
	task_t *win;
	task_t *pin;
	task_t *pout;

	int (*p_callback)(head_t *);

	pthread_t mtid;
	pthread_t ptid;
	pthread_t rtid;
	pthread_t wtid;
};

extern struct svr g_svr;

#endif
