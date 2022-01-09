CC = gcc
CFLAGS = -Wall -g -std=99 -pedantic
LDLIBS = -pthread -lrt #for linux

ALL = lpc_client serveur

all : $(ALL)

init_cond.o : init_cond.c
lpc_client.o : lpc_client.c
serveur.o : serveur.c
prefix_slash.o : prefix_slash.c
thread_error.o : thread_error.c

lpc_client : thread_error.o prefix_slash.o init_cond.o lpc_client.o
serveur : thread_error.o prefix_slash.o init_cond.o serveur.o

clean : rm -rf *~ $(ALL) *.o
