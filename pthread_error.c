#include "pthread_error.h"

/* Display an error message for pthread library functions as follows: 
- code : the error's code
- file : the error's file location
- line : the error's line location
- txt : some text linked to the error or NULL
*/

void thread_error(const char* file, int line, int code , char* txt)
{
  if(txt!=NULL)
    fprintf(stderr, "[pid %d] [%s] in file %s in line %d : %s\n", (int) getpid(), txt, file, line, stderror(code));
  else
    fprintf(stderr, "[pid %d] in file %s in line %d : %s\n", (int) getpid(), file, line, stderror(code));
  exit(1);
}
