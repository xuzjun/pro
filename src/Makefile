BASE_HOME=/home/userx/workspace/pro
ALLFILE= bu.c bu.h cmd.c cmd.h comm.c comm.h mp.c mp.h net.c net.h svr.c svr.h

all: svr

svr: $(ALLFILE)
	gcc -Wall -g $(ALLFILE) -o svr -lpthread
	mv svr $(BASE_HOME)/
