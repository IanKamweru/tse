CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

all:			pageio_test indexio_test lqueue_stest lqueue_dtest

pageio_test:
				gcc $(CFLAGS) pageio_test.c $(LIBS) -o $@

indexio_test:
				gcc $(CFLAGS) indexio_test.c $(LIBS) -o $@

lqueue_stest:
				gcc $(CFLAGS) lqueue_stest.c $(LIBS) -o $@

lqueue_dtest:
				gcc $(CFLAGS) lqueue_dtest.c $(LIBS) -o $@

clean: 
				rm -f *.o pageio_test indexio_test lqueue_stest lqueue_dtest
				