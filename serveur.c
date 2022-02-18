#ifdef __linux__
#define  _XOPEN_SOURCE 500
#endif

#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>

#include "prefix_slash.h"
#include "panic.h"
#include "lpc_type.h"
#include "thread_error.h"
#include "memory.h"
#include "panic.h"
#include "serveur.h"
#include "init_cond.h"
#include "thread_error.h"

#define LEN 256
#define CAPACITE 4096



int main(int argc, char *argv[]){

	//début de l'exemple du cours
	if(argc != 2){
		fprintf(stderr,"%s shared_memory_object\n", argv[0]);
		exit(1);
	}
	//printf("%ld",sysconf(_SC_PAGESIZE)); //taille de page : 4096 



	memory *mem=lpc_create(argv[1],CAPACITE); //mettre la capacité en argv[2] ?
	//int capacite = argv[2];
	if( mem == NULL ){
		PANIC_EXIT("lpc_create");
	}
	printf("Serveur opérationnel\nlancez des clients\n");
	
	//######################################################################################################
	//remplissage du tableau de fonctions déclaré dans serveur.h
	//fptr est un type défini dans lpc_type.h qui désigne un pointeur de fonction de type int fonction(void*)
	initFunTab();
	//######################################################################################################
	
	
	


	while( 1 ){
		int code;
		sleep(1);
		/*  mutex_lock */
		code = pthread_mutex_lock(&(mem->mutex));
		if( code > 0 ){
			thread_error( __FILE__ , __LINE__ , code, "mutex_lock" );
		}
		// On attend qu'un client nous réveille par un pthread_cond_signal(scond) et qu'il y ait une intruction dans la mémoire partagée
		while( mem->libre ){
			printf("Attente de requète\n");
			code = pthread_cond_wait( &(mem->scond), &(mem->mutex));
			if( code > 0 ){
				thread_error( __FILE__, __LINE__, code, "pthread_cond_wait" );
			}
			if(msync( mem, 4096, MS_SYNC) < 0){
				PANIC_EXIT("msync");
			}
			
		}

		//#########################################################################################################
		//############################################ Partie critique ############################################
		//#########################################################################################################
		
		// lecture des données en mémoire
		printf("Le serveur travaille\n");
		
		
		
		// recherche de la fonction appelée (str_compare pour rechercher le nom de la fonction)
		lpc_string *funName=(lpc_string*)( (char*)mem + sizeof(memory) ); //Récupération du lpc_string qui contient le nom de la fonction
		//int (*fonction)(void*)=findFuncPtr(funName->string); //Récupération du pointeur de la fonction
		if(strcmp(funName->string,"init")==0){
			pid_t mem_pid=mem->pid;
			mem->libre=2;
			// levée du verrou mutex
			code = pthread_mutex_unlock(&(mem->mutex));
			if( code > 0){
				thread_error( __FILE__ , __LINE__ , code, "mutex_unlock" );
			}
			pid_t pid=fork(); //création enfant
			if(pid<0){
				PANIC_EXIT("fork");
			}
			if(pid==0){ //côté enfant
				pid_t pid2=fork();
				if(pid2>0){
					printf("le père intermédiaire se suicide\n"); 
					raise(SIGKILL);
				}
				if(pid2<0){
					PANIC_EXIT("fork");
				}
				//On est cette fois dans le petit enfant désormais orphelin (sera tué automatiquement par son père adoptif init)
				char shm_work_name[15]={0};
				sprintf(shm_work_name,"%s%d","mem",mem_pid);
				memory *mem_co = lpc_create(shm_work_name,CAPACITE);
				if( mem_co == NULL ){
					PANIC_EXIT("lpc_create");
				}
				//shm de travail créé
				//réveiller le client
				//Synchronisation "forcée" du shm avec la projection en mémoire avant de redonner la main au client
				if(msync( mem_co, CAPACITE, MS_SYNC) < 0){
					PANIC_EXIT("msync");
				}
				// envoi du signal à ccond pour que le client se réveille envoie les instructions
				printf("\nClient, réveille toi\n");
				code = pthread_cond_signal( &mem->ccond ); //réveille le client pour lui signaler qu'il peut aller lire la réponse en mémoire partagée
				if( code > 0 ){
					thread_error( __FILE__ , __LINE__ , code, "mutex_lock" );
				}
				//client réveillé, peut maintenant ouvrir le shm, et écrire ses requettes dedans.
				/*  mutex_lock */
				code = pthread_mutex_lock(&(mem_co->mutex));
				if( code > 0 ){
					thread_error( __FILE__ , __LINE__ , code, "mutex_lock" );
				}
				// On attend que le client nous réveille à nouveau par un pthread_cond_signal(scond) mais cette fois sur le shm de travail
				while( mem_co->libre ){
					printf("Attente de requète, petit enfant\n");
					code = pthread_cond_wait( &(mem_co->scond), &(mem_co->mutex));
					if( code > 0 ){
						thread_error( __FILE__, __LINE__, code, "pthread_cond_wait" );
					}
					if(msync( mem_co, 4096, MS_SYNC) < 0){
						PANIC_EXIT("msync");
					}	
				}
				//récupération du nom de la fonction à exécuter
				lpc_string *funName_work=(lpc_string*)( (char*)mem_co + sizeof(memory) ); //Récupération du lpc_string qui contient le nom de la fonction
				int (*fonction_work)(void*)=findFuncPtr(funName_work->string); //Récupération du pointeur de la fonction
				if(fonction_work==NULL){
					mem_co->erreur=EFAULT; //Fonction non trouvée , erreur bad adress
				}else{ //exécution de la fonction
					char* paramPtr= (char*)mem_co + sizeof(memory) + sizeof(lpc_string) + funName_work->slen; // On calibre le pointeur paramPtr pour qu'il pointe sur le premier des paramètres à passer à la fonction
					
					mem_co->retourFonction=(*fonction_work)(paramPtr); //Execution de la fonction avec écriture de la valeur de retour à la place dédiée dans la mémoire
					if(mem_co->retourFonction!=0){ 
						mem_co->erreur=errno;
						errno=0;
						//mem->retourFonction=-1;
					}
					
				}
				//la fonction est exécutée (ou pas si elle n'a pas été trouvée)
				
				//Synchronisation "forcée" du shm avec la projection en mémoire avant de redonner la main au client
				if(msync( mem_co, CAPACITE, MS_SYNC) < 0){
					PANIC_EXIT("msync");
				}
				// levée du verrou mutex
				code = pthread_mutex_unlock(&(mem_co->mutex));
				if( code > 0){
					thread_error( __FILE__ , __LINE__ , code, "mutex_unlock" );
				}
				
				// envoi du signal à ccond pour que le client se réveille et récupère la réponse du serveur	
				printf("\nClient, réveille toi\n");
				mem_co->libre=2; //signale au client que la réponse se trouve bien en mémoire et qu'elle peut la lire
				
				code = pthread_cond_signal( &(mem_co->ccond) ); //réveille le client pour lui signaler qu'il peut aller lire la réponse en mémoire partagée
				if( code > 0 ){
					thread_error( __FILE__ , __LINE__ , code, "mutex_lock" );
				}
				
				raise(SIGKILL);
			}//fin du code de l'enfant	
		}else{
			mem->erreur=EFAULT; //init n'a pas été appelé erreur bad adress	
		}
		
		
		
		



		
		
		//Synchronisation "forcée" du shm avec la projection en mémoire avant de redonner la main au client
		if(msync( mem, CAPACITE, MS_SYNC) < 0){
			PANIC_EXIT("msync");
		}
		
	


	}

	

}

void *lpc_create(const char *mem_objet,size_t capacite){
	//int creation = 0;
	
	char *shm_name = prefix_slash(mem_objet);

	/* supprimer l'objet mémoire avant de le récréer */
	shm_unlink(shm_name);

	/* open and create */
	int fd = shm_open(shm_name, O_CREAT| O_RDWR, S_IWUSR | S_IRUSR);
	if( fd < 0 ){
		PANIC_EXIT("shm_open");
	}
	if( ftruncate( fd, capacite ) < 0 ){
		PANIC_EXIT("ftruncate");
	}
	
	/* projection de shared memory object dans
	* la mémoire                              */
	memory *mem = mmap(NULL, capacite, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if( (void *)mem == MAP_FAILED ){
		PANIC_EXIT("mmap");
	}
	

	//initialiser la mémoire
	int code;
	mem->libre = 1;/* memoire est libre initialement*/
	
	code = initialiser_mutex( &(mem->mutex) );
	if( code > 0 ){
		thread_error(__FILE__, __LINE__, code, "init_mutex");
	}
	/* C'est chacun des client qui s'occupera d'initialiser son propre cond
	code = initialiser_cond( &mem->ccond );
	if( code != 0 ){
		thread_error(__FILE__, __LINE__, code, "init_ccond");
	}
	*/


	//initialisation du cond serveur, ne sera plus modifié par la suite
	code = initialiser_cond( &(mem->scond) );
	if( code != 0 ){
		thread_error(__FILE__, __LINE__, code, "init_scond");
	}
	mem->retourFonction=0;
	return mem;
}


int initialiser_mutex(pthread_mutex_t *pmutex){
	pthread_mutexattr_t mutexattr;
	int code;
	if( ( code = pthread_mutexattr_init(&mutexattr) ) != 0){	    
		return code;
	}
	if( ( code = pthread_mutexattr_setpshared(&mutexattr, PTHREAD_PROCESS_SHARED) ) != 0){
		return code;
	}
	code = pthread_mutex_init(pmutex, &mutexattr)  ;
	return code;
}




