#ifndef PTHREADERR
#define PTHREADERR

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

extern void thread_error(const char* file, int line, int code, char* txt);

#endif
