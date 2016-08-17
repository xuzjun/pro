#ifndef _CONN_H_
#define _CONN_H_

extern int create_listen_fd(short port);
extern void *readn(int fd);

#endif
