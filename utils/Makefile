CFLAGS=-Wall -pedantic -std=c11 -I. -g
OFILES=queue.o hash.o webpage.o 

all:	        $(OFILES)
				ar cr ../lib/libutils.a $(OFILES)

%.o:			%.c %.h
				gcc $(CFLAGS) -c $<

webpage.o:		webpage.c webpage.h
				gcc $(CFLAGS) -c webpage.c

webpage:		webpage.o queue.o hash.o 
				gcc $(OFILES) webpage.o queue.o hash.o -o $@
				
queue_test.o:	queue_test.c queue.h
				gcc $(CFLAGS) -c queue_test.c

queue_test:		queue_test.o queue.o
				gcc $(CFLAGS) queue_test.o queue.o -o $@

hash_test.o:	hash_test.c hash.h
				gcc $(CFLAGS) -c hash_test.c

hash_test:		hash_test.o hash.o queue.o
				gcc $(CFLAGS) hash_test.o hash.o queue.o -o $@

clean:			
				rm -f *.o queue_test hash_test
