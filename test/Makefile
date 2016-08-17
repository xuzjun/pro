BASE_HOME=/home/userx/workspace/pro
all: Test

Test: client.c
	gcc -Wall -g -I$(BASE_HOME)/src client.c -o Test -lpthread
	mv Test $(BASE_HOME)/
