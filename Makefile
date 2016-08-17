all: test svr

test: nothing
	make -C test

svr: nothing
	make -C src

nothing:
