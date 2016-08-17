#include "bu.h"
#include "comm.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

void to_upper_s(char *s, int len)
{
	while (len--) {
		*s = toupper(*s);
		s++;
	}
}

void to_lower_s(char *s, int len)
{
	while (len--) {
		*s = tolower(*s);
		s++;
	}
}

int p_cb(head_t *h)
{
	int l = h->content_len;
	char *buff = calloc(1, l + 1);
	memcpy(buff, h + 1, l);
	printf("p. in p_cb buff[%s]\n", buff);

	to_upper_s((char *)(h + 1), l);

	return 0;
}

int to_upper(head_t *h)
{
	int l = h->content_len;
	char *buff = calloc(1, l + 1);
	memcpy(buff, h + 1, l);
	printf("p. in p_cb buff[%s]\n", buff);

	to_upper_s((char *)(h + 1), l);

	return 0;
}

int to_lower(head_t *h)
{
	int l = h->content_len;
	char *buff = calloc(1, l + 1);
	memcpy(buff, h + 1, l);
	printf("p. in p_cb buff[%s]\n", buff);

	to_lower_s((char *)(h + 1), l);

	return 0;
}
