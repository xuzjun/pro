#include "svr.h"
#include "bu.h"
#include "mp.h"
#include "net.h"
#include <stdio.h>

struct svr g_svr;

static int server_init()
{
	g_svr.rin = task_init();
	g_svr.rout = task_init();
	g_svr.win = task_init();
	g_svr.pin = task_init();
	g_svr.pout = task_init();

	g_svr.p_callback = p_cb;

	g_svr.port = PORT;
	if ((g_svr.listenfd = create_listen_fd(PORT)) == -1) {
		printf("create listen fd error");	
		return -1;
	}
	mp_run();
	return 0;
}

static void server_join()
{
	mp_join();
}

int main(int argc, char *argv[])
{
	if (server_init(argc, argv) == -1)
		return -1;
	printf("%ld\n%ld\n%ld\n%ld\n", g_svr.mtid, g_svr.ptid, g_svr.rtid, g_svr.wtid);
	server_join();
	return 0;
}
