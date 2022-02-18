#ifndef LPC_TYPE_H
#define LPC_TYPE_H
#define NAMELEN 48

typedef enum{STRING,DOUBLE,INT,NOP} lpc_type;

typedef struct{
  int slen;
  char string[];
} lpc_string;



typedef int (*fptr)(void*);

typedef struct{
	char fun_name[NAMELEN];
	int (*fun)(void*);
}lpc_function;

#endif

