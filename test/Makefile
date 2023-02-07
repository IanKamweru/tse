CFLAGS=-Wall -pedantic -std=c11 -I../utils -L../lib -g
LIBS=-lutils -lcurl

pageio_test:
				gcc $(CFLAGS) pageio_test.c $(LIBS) -o $@

clean: 
				rm -f *.o pageio_test
