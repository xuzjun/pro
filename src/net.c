#include "net.h"
#include "comm.h"
#include "svr.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#define  ERROR_RET \
	close(fd); \
        return -1

int create_listen_fd(short port)
{
	int fd;
	if ((fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) {
		perror("socket error");	
		return -1;
	}

	int optval = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		perror("setsockopt error");
		ERROR_RET;
	}

	struct sockaddr_in svraddr;
	memset(&svraddr, 0x00, sizeof(svraddr));
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	svraddr.sin_port = htons(port);
	if (bind(fd, (struct sockaddr *)&svraddr, sizeof(svraddr)) == -1) {
		perror("bind error");
		ERROR_RET;
	}
	if (listen(fd, MAXLISTEN) == -1) {
		perror("listen error");	
		ERROR_RET;
	}
	return fd;
}

static int read_(int fd, void *b, int l)
{
	int nleft = l;
	int n;
	char *ptr = (char *)b;
	while (n) {
		n = read(fd, ptr, nleft);
		if (n == 0)	
			break;
		if (n == -1) {
			if (errno == EAGAIN || errno == EBUSY || errno == EWOULDBLOCK) {
				continue;
			} else {
				printf("r. read fd[%d] error, errno[%d]\n", fd, errno);
				return -1;
			}
		}

		ptr += n;
		nleft -= n;
	}
	return l - nleft;
}

void *readn(int fd)
{
	int ret;
	head_t *h = calloc(1, sizeof(head_t));
	ret = read_(fd, (void *)h, sizeof(head_t));
	if (ret != sizeof(head_t)) {
		printf("r. read fd[%d] head error, ret[%d]\n", fd, ret);
		return NULL;
	}

	if (h->magic_num != MAGICNUM) {
		printf("r. magic number[%d] error\n", h->magic_num);
		return NULL;
	}
	printf("r. readn content_len[%d]\n", h->content_len);
	head_t *h2 = calloc(1, sizeof(head_t) + h->content_len + sizeof(int));
	ret = read_(fd, (void *)(h2 + 1), h->content_len);
	if (ret != h->content_len) {
		printf("r. read fd[%d] content error, ret[%d]\n", fd, ret);
		return NULL;
	}
	memcpy(h2, h, sizeof(head_t));
	*(int *)((char *)(h2 + 1) + h->content_len) = fd;
	return h2;
}
