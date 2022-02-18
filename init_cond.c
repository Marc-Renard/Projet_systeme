 #include <pthread.h>
#include "init_cond.h"


int initialiser_cond(pthread_cond_t *pcond){
	pthread_condattr_t condattr;
	int code;
	if( ( code = pthread_condattr_init(&condattr) ) != 0 ){
		return code;
	}
	if( ( code = pthread_condattr_setpshared(&condattr,PTHREAD_PROCESS_SHARED) ) != 0 ){
		return code;
	}
	code = pthread_cond_init(pcond, &condattr) ; 
	return code;
}	
