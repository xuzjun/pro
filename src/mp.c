#include "mp.h"
#include "svr.h"
#include "net.h"
#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>


static int accept_fd(int fd)
{
	int *t = calloc(2, sizeof(int));
	*t = FD_IN;
	*(t + 1) = fd;
	return task_push(g_svr.rin, t);
}


static void *fm(void *p)
{
	printf("I am thread m\n");
	int listenfd = g_svr.listenfd;
	int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	if (epoll_fd == -1) {
		perror("epoll_create1 error");
		return NULL;	
	}
	struct epoll_event ev, events[MAXEVENTS];
	memset(&ev, 0x00, sizeof(ev));
	memset(events, 0x00, MAXEVENTS * sizeof(struct epoll_event));
	ev.events = EPOLLIN;
	ev.data.fd = listenfd;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listenfd, &ev) == -1) {
		perror("m. epoll_ctl error");	
		return NULL;
	}
	int nfds;
	struct sockaddr_in cliaddr;
	int fd;
	int i;
	int l = sizeof(cliaddr);
	int ret;
	while (1) {
		nfds = epoll_wait(epoll_fd, events, MAXEVENTS, 0);
		if (nfds < 0) {
			perror("m. epoll_wait error");		
			continue;
		}
		for (i = 0; i < nfds; ++i) {
			if (events[i].data.fd == listenfd) {
				while (1) {
					fd = accept(listenfd, (struct sockaddr *)&cliaddr, (socklen_t *)&l);
				 	if (fd == -1 && errno == EAGAIN) {
						break;
					} else if (fd > 0) {
						printf("m. accept fd[%d] from %s\n", fd, inet_ntoa(cliaddr.sin_addr));
						set_fd_nonblock(fd);
						ret = accept_fd(fd);
						if (ret)
							fprintf(stderr, "m. accept fd error[%d]\n", ret);
					}
				}	
			}
		}

		void *t;
		if (task_pop(g_svr.rout, &t) == 0) {
			int *ti = (int *)t;
			if (*ti == FD_ERR) {
				fd = *(ti + 1);
				close(fd);
				printf("m. close fd [%d]\n", fd);
			}
			if (*ti == MAGICNUM) {
				head_t *h = (head_t *)t;
				printf("m. get len[%d] content[%s]\n", h->content_len, (char *)(h + 1));
				if (task_push(g_svr.pin, t) != 0)
					fprintf(stderr, "m. push data to pin error\n");
			}
		}
		if (task_pop(g_svr.pout, &t) == 0) {
			int *ti = (int *)t;
			if (*ti != MAGICNUM)
				fprintf(stderr, "m. magic error from pout\n");
			else
				if (task_push(g_svr.win, t) != 0)
					fprintf(stderr, "m. push data to win error\n");

		}

		usleep(2);
	}
	return NULL;
}

static void *fp(void *p)
{
	printf("I am thread p\n");

	void *t;
	int  ret;
	while (1) {
		if (task_pop(g_svr.pin, &t) == 0) {
			int *ti = (int *)t;
			if (*ti != MAGICNUM) {
				fprintf(stderr, "p. magic number [%d] error\n", *ti);
				continue;
			}
			ret = g_svr.p_callback((head_t *)t);
			if (ret == 0) {
				printf("p. p_callback return\n");
				if (task_push(g_svr.pout, t) != 0)
					printf("p. push data error\n");
			}
		}
		usleep(2);
	}
	return NULL;
}

static void *fr(void *p)
{
	printf("I am thread r\n");

	int epoll_fd = epoll_create1(EPOLL_CLOEXEC);
	struct epoll_event ev, events[MAXEVENTS];
	memset(events, 0x00, MAXEVENTS * sizeof(struct epoll_event));

	head_t *buff;
	void *t = NULL;
	int nfds;
	int i;
	while (1) {
		if (task_pop(g_svr.rin, &t) == 0){
			if (*(int *)t == FD_IN) {
				int fd = *((int *)t + 1);
				memset(&ev, 0x00, sizeof(ev));
				free(t);
				ev.events = EPOLLIN | EPOLLHUP;
				ev.data.fd = fd;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
					printf("epoll add fd[%d] error\n", fd);
					int *ti = calloc(2, sizeof(int));
					*ti = FD_ERR;
					*(ti + 1) = fd;
					task_push(g_svr.rout, (void *)ti);
				}
				printf("r. add fd[%d]\n", fd);
			}
		}
		nfds = epoll_wait(epoll_fd, events, MAXEVENTS, 0);
		for (i = 0; i < nfds; i++) {
			if (events[i].events & EPOLLIN) {
				printf("r. event EPOLLIN arrival\n");
				buff = (head_t *)readn(events[i].data.fd);
				if (buff != NULL) {
					task_push(g_svr.rout, (void *)buff);
				} else {
					epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
					int *ti = calloc(2, sizeof(int));
					*ti = FD_ERR;
					*(ti + 1) = events[i].data.fd;
					task_push(g_svr.rout, (void *)ti);
					printf("r. delete fd [%d] from epoll\n", events[i].data.fd);
				}
			}
			if (events[i].events & EPOLLHUP) {
				printf("r. event EPOLLHUP arrival\n");
				epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL);
				int *ti = calloc(2, sizeof(int));
				*ti = FD_ERR;
				*(ti + 1) = events[i].data.fd;
				task_push(g_svr.rout, (void *)ti);
				printf("r. delete fd [%d] from epoll\n", events[i].data.fd);
			}
		}
	}
	return NULL;
}

static void *fw(void *p)
{
	printf("I am thread w\n");
	void *t;
	int  n;
	while (1) {
		if (task_pop(g_svr.win, &t) == 0) {
			head_t *h = (head_t *)t;
			if (h->magic_num != MAGICNUM) {
				fprintf(stderr, "w. magic number error");
				continue;
			}
			int fd = *(int *)((char *)(h + 1) + h->content_len);
			printf("w. will write to fd[%d]\n", fd);
			n = write(fd, (void *)h, sizeof(head_t) + h->content_len);
			printf("w. write length [%d] to fd\n", n);
			free(t);
		}
	
		usleep(2);
	}
	return NULL;
}

int mp_run()
{
	pthread_create(&g_svr.mtid, NULL, fm, NULL);
	pthread_create(&g_svr.ptid, NULL, fp, NULL);
	pthread_create(&g_svr.rtid, NULL, fr, NULL);
	pthread_create(&g_svr.wtid, NULL, fw, NULL);
	return 0;
}

int mp_join()
{
	pthread_join(g_svr.mtid, NULL);
	pthread_join(g_svr.ptid, NULL);
	pthread_join(g_svr.rtid, NULL);
	pthread_join(g_svr.wtid, NULL);
	return 0;
}
