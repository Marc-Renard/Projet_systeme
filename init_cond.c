#include "init_cond.h"
#define L_NAME 256

/* This file is dedicated to initializing the condition variable. It ll be used by the server and the clients to interact between them */

int initialiser_cond(pthread_cond_t * pcond)
{
  pthread_condattr_t condattr;
  int code;
  if((code = pthread_condattr_init(&condattr)) != 0)
     return code;
  if((code ) pthread_condattr_setpshared(&condattr,PTRHEAD_PROCESS_SHARED))!=0)
    return code;
  code = pthread_cond_init(pcond, &condattr);
  return code;
}

/*prefix slash allows to get the path of an object*/

char *prefix_slash(const char* name)
{
  char *nom = malloc(L_NAME);
  if(name[1] != '/')
  {
    nom[0] = '/';
    nom[1] = '\0';
    strncat(nom,name,L_NAME+3);
  }
  else
  {
    strncpy(nom,name,L_NAME);
    nom[L_NAME-1] = '\0';
  }
  return nom;
}
