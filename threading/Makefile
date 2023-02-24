CFLAGS=-Wall -pedantic -std=c11 -I. -g

all:	        lqueue_stest lqueue_dtest

%.o:			%.c %.h
				gcc $(CFLAGS) -c $<

lqueue_stest.o:	lqueue_stest.c lqueue.h queue.h
				gcc $(CFLAGS) -c lqueue_stest.c

lqueue_stest:	lqueue_stest.o lqueue.o queue.o
				gcc $(CFLAGS) lqueue_stest.o lqueue.o queue.o -o $@

lqueue_dtest.o:	lqueue_dtest.c lqueue.h queue.h
				gcc $(CFLAGS) -c lqueue_dtest.c

lqueue_dtest:	lqueue_dtest.o lqueue.o queue.o
				gcc $(CFLAGS) lqueue_dtest.o lqueue.o queue.o -o $@

clean:			
				rm -f *.o lqueue_stest lqueue_dtest
