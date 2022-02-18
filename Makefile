CC=gcc
CFLAGS=-Wall -g -std=c99 -pedantic #-fsanitize=leak
LDLIBS=-lrt -pthread  #linux



ALL =   lpc_client serveur

all : $(ALL)



init_cond.o: init_cond.c
lpc_client.o: lpc_client.c
serveur.o: serveur.c
prefix_slash.o: prefix_slash.c

thread_error.o : thread_error.c

lpc_client : thread_error.o prefix_slash.o lpc_client.o init_cond.o
serveur : serveur.o thread_error.o init_cond.o prefix_slash.o


clean:
	rm -rf *~ $(ALL) *.o
