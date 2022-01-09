#ifndef PCOND
#define PCOND

#include <pthread.h>
#include <string.h>
#include <stdlib.h>

int initialiser_cond(pthread_cond_t * pcond);

/*====================================================
The memory header
====================================================*/

typedef struct{
  pthread_mutex_t mutex;
  pthread_cond_t ccond; //client cond
  pthread_cond_t scond; //server cond
  pid_t pid;
  int erreur;
  int libre;
  int retourFonction; // 0 if everything s alright, -1 if not
}memory; //Data are stored just after the header, as we overmap the shm for this purpose

/*===================================================
The prefix slash function, that allows to right the object path easier
===================================================*/

char *prefix_slash(const char* name);

#endif
