/*
This file regroups the different structures we will use as data : String will be converted to lpc_string.
Also there will be a special type for functions : lpc_function
*/
#ifndef LPC_TYPE
#define LPC_TYPE

#define NAMELEN 48

typedef enum{INT,DOUBLE,STRING,NOP}lpc_type;

typedef struct{
  int slen;
  char string[];
}lpc_string;

typedef int (*fptr)(void*);

typedef struct{
  char fun_name[NAMELEN];
  int (*fun)(void*);
}lpc_function;

#endif
