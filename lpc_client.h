#ifndef LPC_CLIENT
#define LPC_CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "lpc_type.h"


void* lpc_open(const char * name);                            //open the client side of the shm
int lpc_close(void* mem);                                     //close the client side of the shm
lpc_string* lpc_make_string(const char * s, int taille);      //create a lpc_string object
int lpc_call(void* memory, const char* fun_name, ... );       //client calls the server on a shm for a specific function

#endif
