/*==================================================
Header for the server part
==================================================*/
#ifndef LINUX
#define XOPEN_SOURCE 500
#endif

#ifndef SERVER
#define SERVER

#include "init_cond.h"
#include "lpc_type.h"
#include "panic.h"
#include "pthread_error.h"

#include <fnctl.h>
#include <sys/stat.h>
#include <sys/type.h>
#include <sys/mman.h>
#include <signal.h>
#include <errno.h>


//#define NAMELEN 48 already define in lpc_type
#define LEN 256
#define CAPACITY 4096


#define NB_FUN 3          //to be updated if you wish to add more functions
//include new functions below
#include "trunc.h"
#include "concat.h"
#include "div.h"

lpc_function fun_tab[NB_FUN];

void init_fun_tab()           //init of functions table
{
  fptr f1 = &trunc;
  const lpc_function truncat = {"truncate", f1};
  fun_tab[0] = truncat;
  
  fptr f2 = &concat;
  const lpc_function concat = {"concatenate", f2};
  fun_tab[1] = concat;
  
  fptr f3 = &divide;
  const lpc_function divide = {"division", f3};
  fun_tab[2] = divide;
  
  //to add a new function, first update NB_FUN, then include your header and update init_fun_tab to include the new function following the above format
}

int (*find_fun_ptr(const char *name)) (void*) //check if a name is linked to a function inside the functions table
{
  for(int i = 0; i < NB_FUN; i++)
  {
    if(strcmp(fun_tab[i].fun_name,name) ==0)
      return fun_tab[i].fun;
  }
  return NULL;
}

void* lpc_create(const char* mem_object, size_t capacity);
int init_mutex(pthread_mutex_t* pmutex);
