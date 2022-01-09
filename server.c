#include "server.h"

int init_mutex(pthread_mutex_t* pmutex)
{
  pthread_mutexattr_t mutexattr;
  int code;
  
  if((code = pthread_mutexattr_init(&mutexattr))!=0)
    return code;
  
  if((code = pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED))!=0)
    return code;
  
  code = pthread_mutex_init(pmutex, &mutexattr);
  return code;
}

void* lpc_create(const char* mem_object, size_t capacity)
{
  char* shm_name = prefix_slash(mem_object);
  
  shm_unlink(mem_object);
  
  int fd = shm_open(shm_name, O_CREAT | O-RDWR, S_IWUSR | S_IRUSR);
  if(fd <0)
    PANIC_EXIT("shm_open");
  
  if(ftruncate(fd, capacity) < 0)
    PANIC_EXIT("ftruncate");
  
  memory* mem = mmap(NULL , capacity , PROT_READ | PROT_WRITE, MAP_SHARED , fd, 0);
  if((void*)mem == NULL)
    PANIC_EXIT("mmap");
  
  int code;
  mem->libre = 1;
  
  code = init_mutex(&(mem->mutex));
  if(code > 0)
    thread_error(__FILE__,__LINE__,code, "init_mutex");
  
  code = initialiser_cond(&(mem->scond));
  if(code !=0)
    thread_error(__FILE__,__LINE__,code,"init_cond");
  
  mem->retourFonction = 0;
  return mem;
}
