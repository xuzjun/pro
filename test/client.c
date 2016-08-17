#include <stdio.h>
#include "comm.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>

#define BUFFSIZE   1024
#define THREADNUM  1024

void *t_f(void *p)
{
	struct timeval tv;
	long long starttm, endtm;
	memset(&tv, 0x00, sizeof(tv));

	int fd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in servaddr;
	memset(&servaddr, 0x00, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
	servaddr.sin_port = htons(9999);

	connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	char *s = "ieruiwoBNZ>DKweiw;p09485=23kfjwoeiurr839OPWjksdjfldkjPQ{:AHFEOOWEKFoq982394837ksdfkdjfie8iwdjfo";
	int len = strlen(s);
	head_t *h = calloc(1, sizeof(head_t) + len + 1);
	h->magic_num = MAGICNUM;
	h->content_len = len;
	memcpy(h + 1, s, len);
	int n = write(fd, (void *)h, len + sizeof(head_t) + 1);
	gettimeofday(&tv, NULL);
	starttm = tv.tv_sec * 1000000 + tv.tv_usec;
	// printf("write. head length[%ld] content[%s] length[%d] write lenght[%d]\n", sizeof(head_t), s, len, n);
	free(h);
	char buff[BUFFSIZE] = {0};
	n = read(fd, buff, BUFFSIZE);
	h = (head_t *)buff;
	memset(&tv, 0x00, sizeof(tv));
	gettimeofday(&tv, NULL);
	endtm = tv.tv_sec * 1000000 + tv.tv_usec;
	// printf("read.  lenght[%d] magic number [%d] content length[%d] content[%s]\n", n, h->magic_num, h->content_len, (char *)(h + 1));
	printf("thread. start:%lld: end:%lld: :%lld:\n", starttm, endtm, endtm - starttm);
	close(fd);
	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stdout, "Usage: argv[0] <param>");
		return -1;
	}

	int i;
	pthread_t tid[THREADNUM];
	for (i = 0; i < THREADNUM; i++) {
		pthread_create(&tid[i], NULL, t_f, NULL);	
	}

	for (i = 0; i < THREADNUM; i++) {
		pthread_join(tid[i], NULL);	
	}

	return 0;
}
