#ifndef LPC_CLIENT_H
#define LPC_CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "lpc_type.h"

void *lpc_open(const char *name);
int lpc_close(void *mem);
lpc_string * lpc_make_string(const char *s, int taille);
int lpc_call(void *memory,const char *fun_name, ...);




#endif
