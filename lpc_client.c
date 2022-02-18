#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>

#include "prefix_slash.h"
#include "panic.h"
#include "lpc_type.h"
#include "thread_error.h"
#include "lpc_client.h"
#include "memory.h"
#include "init_cond.h"


#define LEN 256
static int taille_mem=0;

int main(int argc, char **argv){
	if(argc != 3){
		fprintf(stderr,"%s shared_memory_object numTest\n", argv[0]);
		exit(1);
	}
	

	

	//projection en mémoire
	memory  *mem=lpc_open(argv[1]);
	if(mem==NULL){
		PANIC_EXIT("Pas de shm correspondant\n");
	}
	//printf("shm trouvé\nLe client est lancé\n");
	
	
	int code;

	/*  mutex_lock */
	code = pthread_mutex_lock(&(mem->mutex)); //si mutex déjà verrouillé, alors on bloque sur cette instruction jusqu'a ce qu'il soit déverrouillé
	if( code > 0 ){
		thread_error( __FILE__ , __LINE__ , code, "mutex_lock" );
	}
	while( !(mem->libre) ){ //cela veut dire que la mémoire partagée est occupée par une réponse qui s'adresse à un autre client
		pthread_mutex_unlock(&(mem->mutex));
		sleep(5); //on attend 5 secondes que le processus qui attend sa réponse ait le temps de poser son verrou pour récupérer ses données et "remettre à 0" la mémoire partagée
		pthread_mutex_lock(&(mem->mutex));
	}
	//Une fois arrivé à ce stade, le client a le verrou du mutex, et la mémoire partagée est libre (les autres clients potentiels ont récupéré leur réponse du serveur)
	
	//création de son cond
	
	code=initialiser_cond(&(mem->ccond));
	if( code > 0 ){
		thread_error(__FILE__, __LINE__, code, "init_mutex");
	}
	
	//Jusqu'ici, c'est comme le client "normal", on a le verrou, on peut écrire dans le shm
	
	pid_t p=getpid();
	printf("Mon pid est : %d\n\n",p);
	mem->pid=p;
	lpc_call(mem,"init",NOP);
	
	char shm_work_name[15]={0};
	sprintf(shm_work_name,"%s%d","mem",p); //construit le nom du shm qui sera utilisé pour le "travail"
	printf("%s\n",shm_work_name);
	
	//############################################################
	//############################################################
	
	
	//projection en mémoire
	memory  *mem_co=lpc_open(shm_work_name);
	if(mem_co==NULL){
		PANIC_EXIT("Pas de shm correspondant\n");
	}
	//printf("shm trouvé\nLe client est lancé\n");
	

	/*  mutex_lock */
	code = pthread_mutex_lock(&(mem_co->mutex)); //si mutex déjà verrouillé, alors on bloque sur cette instruction jusqu'a ce qu'il soit déverrouillé
	if( code > 0 ){
		thread_error( __FILE__ , __LINE__ , code, "mutex_lock" );
	}
	while( !(mem_co->libre) ){ //cela veut dire que la mémoire partagée est occupée par une réponse qui s'adresse à un autre client
		pthread_mutex_unlock(&(mem_co->mutex));
		sleep(5); //on attend 5 seconde que le processus qui attend sa réponse ait le temps de poser son verrou pour récupérer ses données et "remettre à 0" la mémoire partagée
		pthread_mutex_lock(&(mem_co->mutex));
	}
	//Une fois arrivé à ce stade, le client a le verrou du mutex, et la mémoire partagée est libre (les autres clients potentiels ont récupéré leur réponse du serveur)
	
	//création de son cond
	
	code=initialiser_cond(&(mem_co->ccond));
	if( code > 0 ){
		thread_error(__FILE__, __LINE__, code, "init_mutex");
	}
	
	
	//############################################################
	//############################################################
	
	
	
	
	
	
	

	//switch des différents tests
	int testAExecuter=atoi(argv[2]);
	switch(testAExecuter){
		case 1:
			//######################## test 1 #########################################
			// troncature de firstTest à 5 caractères
			// retour attendu : fisrt
			printf("##### Test 1 : troncature sans erreur\n");
			printf("##### Troncature de firstTest à 5 caractères\n");
			printf("##### Résultat attendu :\n");
			printf("Retour de fonction : 0 (exécution sans erreur)\nfirst\n4 = nombre de caractères retirés\n\n");
			printf("##### Résultat obtenu :\n");
			
			lpc_string *f=lpc_make_string("firstTest",10);
			int a=5;
			lpc_call(mem_co,"troncature",STRING,f,INT,&a,NOP);
			break;
		
			//###################### fin test 1 #######################################
			

		case 2:
			//######################## test 2 #########################################
			// troncature de firstTest à 12 caractères
			// retour attendu : erreur, first fait déjà moins de 12 caractères
			printf("##### Test 2 : troncature avec erreur\n");
			printf("##### Troncature de firstTest à 12 caractères\n");
			printf("##### Résultat attendu :\n");
			printf("Retour de fonction :  -1 (erreur lors de l'exécution)\n\n");
			printf("##### Résultat obtenu :\n");
			
			lpc_string *g=lpc_make_string("firstTest",10);
			int b=12;
			lpc_call(mem_co,"troncature",STRING,g,INT,&b,NOP);
			//###################### fin test 2 #######################################
			
			break;
			

		case 3:
			//######################## test 3 #########################################
			// Taiile s1 ok pour le retour de la fonction
			printf("##### Test 4 : concaténation sans erreur\n");
			printf("##### Concaténation de str1 er chaine2 et le lpc_string qui contient str1 a assez de place pour ajouter chaine2\n");
			printf("##### Résultat attendu :\n");
			printf("Retour de fonction : 0 (exécution sans erreur)\n\nstr1chaine2 \nchaine2\n\n");
			printf("##### Résultat obtenu :\n");
			
			lpc_string *s1=lpc_make_string("str1",15);
			lpc_string *s2=lpc_make_string("chaine2",10);
			lpc_call(mem_co,"concatener",STRING,s1,STRING,s2,NOP);
			//###################### fin test 3 #######################################
			break;
				
		case 4:
			//######################## test 4 #########################################
			// Taile s3 insuffisante pour le retour de la fonction
			printf("##### Test 4 : concaténation avec erreur\n");
			printf("##### Concaténation de str1 er chaine2 mais le lpc_string qui contient str1 n'a pas la place pour ajouter chaine2\n");
			printf("##### Résultat attendu :\n");
			printf("Retour de fonction : -1 (erreur lors de l'exécution)\n\n");
			printf("##### Résultat obtenu :\n");
			
			lpc_string *s3=lpc_make_string("str1",5);
			lpc_string *s4=lpc_make_string("chaine2",10);
			lpc_call(mem_co,"concatener",STRING,s3,STRING,s4,NOP);
			//###################### fin test 4 #######################################
			break;
		case 5:
			//######################## test 5 #########################################
			// Division correcte
			printf("##### Test 5 : division\n");
			printf("##### division de 5.0 par 2.0\n");
			printf("##### Résultat attendu :\n");
			printf("Retour de fonction : 0 (exécution sans erreur)\n\n2.5\n2.0\n");
			printf("##### Résultat obtenu :\n");
			
			double a1=5.0;
			double a2=2.0;
			
			
			lpc_call(mem_co,"diviser",DOUBLE,&a1,DOUBLE,&a2,NOP);
			//###################### fin test 5 #######################################
			break;
		case 6:
			//######################## test 6 #########################################
			// Division par 0
			printf("##### Test 6 : division par 0\n");
			printf("##### division de 5.0 par 0\n");
			printf("##### Résultat attendu :\n");
			printf("Retour de fonction : -1 (erreur lors de l'exécution)\n\n");
			printf("##### Résultat obtenu :\n");
			
			double b1=5.0;
			double b2=0;
			
			
			lpc_call(mem_co,"diviser",DOUBLE,&b1,DOUBLE,&b2,NOP);
			//###################### fin test 6 #######################################
			break;
		case 7:
			//######################## test 7 #########################################
			//Fonction inconnue côté serveur
			printf("##### Test 7 : fonction inconnue coté serveur\n");
			lpc_call(mem_co,"inconnue",NOP);
			//###################### fin test 7 #######################################
			
			break;
		default:
			printf("Test non reconnu\n");			
			break;
	}


	
	
	

	lpc_close(mem);
	lpc_close(mem_co);
	shm_unlink(shm_work_name);
	

}


void *lpc_open(const char *name){
	char *shm_name = prefix_slash(name);
	int fd=shm_open(shm_name, O_RDWR,S_IRUSR|S_IWUSR);
	if ( fd < 0 ){
		printf("Echec de l'ouverture du shm\n");
		return NULL;	
	}
	struct stat st;
	if (fstat(fd, &st))
	{
		PANIC_EXIT("fstat");
	}
	taille_mem=st.st_size;
	void *mem=mmap(NULL,st.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	if(mem==MAP_FAILED){
		PANIC_EXIT("mmap");
	}
	return mem;
}

int lpc_close(void *mem){
	int c=munmap(mem,taille_mem);
	if(c<0){
		PANIC_EXIT(strerror(errno));
	}
	/* Plus éventuellement des supressions de structures que le client aurait pu allouer localement*/
	/* Par exemple si des lpc_string sont créés comme décrit dans 3.3.1*/
	return 0;
}



int lpc_call(void *memory_shared,const char *fun_name, ...){
	
	
	
	va_list args; //variable qui va permettre de récupérer la liste des arguments passés à la fonction engage
	memory *mem=(memory*)memory_shared;
	char *donnees=(char*)mem + sizeof(memory);
	
	//écriture du nom de la fonction au début de la mémoire
	lpc_string *fun_string=lpc_make_string(fun_name,strlen(fun_name)+1);
	memcpy(donnees,fun_string,sizeof(lpc_string) + fun_string->slen);
	donnees = donnees + sizeof(lpc_string) + fun_string->slen;
	
	
	va_start(args,fun_name); //initialisation de la liste d'arguments sur le premier argument
	lpc_type type=va_arg(args,lpc_type);
	int *int_ptr=NULL;
	double *double_ptr=NULL;
	lpc_string *lpc_string_ptr=NULL;
	
	while(type!=NOP){
		
		switch(type){
			case INT:
				int_ptr = va_arg(args,int*);
				//on copie l'entier dans les données
				memcpy(donnees,int_ptr,sizeof(int));
				//on met à jour l'adresse à laquelle écrire les données suivantes
				donnees = donnees + sizeof(int);
				break;
				
			case DOUBLE:
				double_ptr = va_arg(args,double*);
				//on copie le double dans les données
				memcpy(donnees,double_ptr,sizeof(double));
				//on met à jour l'adresse à laquelle écrire les données suivantes
				donnees = donnees + sizeof(double);
				break;
			case STRING:
				//ça
				lpc_string_ptr=va_arg(args,lpc_string*);
				//on copie le lpc_string dans les données
				memcpy(donnees,lpc_string_ptr,sizeof(lpc_string)+lpc_string_ptr->slen);
				//par construction du lpc_string, slen est supérieur ou égal à la taille réelle de string, et de part le memset avec des 0, on va soit copier string0 ou string0...0, ce qui pourra être récupéré côté serveur
				
				
				//Garder en tete que le serveur saura, en fonction de la fonction appelée la forme de la liste d'arguments


				donnees = donnees + sizeof(lpc_string) + lpc_string_ptr->slen;
				break;
			default:
				PANIC_EXIT("Unknown type\n");
				break;
		}

		type=va_arg(args,lpc_type);	// taille suivante dans la liste
	}
	va_end(args);
	//les données à envoyer au serveur sont désormais dans la mémoire paratagée
	//printf("fin lecture\n");

	//Un autre client "nouveau" ne pourra pas "voler" le verrou car libre n'est pas à 1
	//Et un autre client en attente de réponse ne peut pas se réveiller sans un signal du serveur, qui lui gère les client un par un, donc pas de risque qu'un autre client "vole" le verrou et bloque l'exécution
	mem->libre=0;
	int code;
	//Synchronisation "forcée" du shm avec la projection en mémoire avant de donner la main au serveur
	if(msync( mem, 4096, MS_SYNC) < 0){
		PANIC_EXIT("msync");
	}
	code = pthread_mutex_unlock(&(mem->mutex)); //on déverrouille le mutex
	//printf("Mutex libéré\n");
	if( code > 0 ){
			thread_error( __FILE__ , __LINE__ , code, "mutex_unlock" );
	}
	
	code = pthread_cond_signal(&(mem->scond)); //On réveille le serveur pour qu'il traite notre demande
	//printf("Serveur réveille toi\n");
	if( code > 0 ){
			thread_error( __FILE__ , __LINE__ , code, "pthread_cond_signal" );
	}
	
	//printf("Attente de réponse du serveur\n");
	//englober dans un while avec condition
	while(mem->libre!=2){ //libre = 2 indique que le serveur n'a aps récupéré le pid
		code = pthread_cond_wait(&(mem->ccond),&(mem->mutex)); //On attend que le serveur nous réveille une fois que la réponse à notre requette se trouvera dans la mémoire partagée
		if( code > 0 ){
			thread_error( __FILE__ , __LINE__ , code, "pthread_cond_wait" );
		}
	}

	
	
	
	//######################## Traitement des réponses du serveur ###############################
	//printf("Lecture des réponses du serveur\n\n");
	
	printf("Retour de fonction : %d\n\n",mem->retourFonction);
	if(mem->erreur==0){
		//Nous allons relire la liste des argument pour trouver la chaine de caractère problèmatique
		va_list args2;
		va_start(args2,fun_name);
		type=va_arg(args2,lpc_type);
		
		donnees=(char*)mem + sizeof(memory) + sizeof(lpc_string) + fun_string->slen; //on replace donnees sur le premier des paramètres, ie après la structure memory et après le lpc string qui contient le nom de la fonction appelée
		
		while(type!=NOP){ 
			switch(type){
				case INT:
					int_ptr = va_arg(args2,int*);//il faut faire passer à l'arg suivant pour que le prochain soit bien un type
					int_ptr = (int*)donnees;
					//affichage
					printf("%d\n",*int_ptr);
					//On efface la zone mémoire qui contenait ce int
					memset(int_ptr,0,sizeof(int));

					//On efface la zone mémoire qui contenait ce int
					memset(int_ptr,0,sizeof(int));
					//on met à jour l'adresse à laquelle lire les données suivantes
					donnees = donnees + sizeof(int);
					break;
					
				case DOUBLE:
					double_ptr = va_arg(args2,double*); //il faut faire passer à l'arg suivant pour que le prochain soit bien un type
					double_ptr = (double*)donnees;
					//affichage
					printf("%f\n",*double_ptr);
					//On efface la zone mémoire qui contenait ce double
					memset(double_ptr,0,sizeof(double));
					
					//On efface la zone mémoire qui contenait ce double
					memset(double_ptr,0,sizeof(double));
					//on met à jour l'adresse à laquelle écrire les données suivantes
					donnees = donnees + sizeof(double);
					break;
				case STRING:
					lpc_string_ptr = va_arg(args2,lpc_string*); //il faut faire passer à l'arg suivant pour que le prochain soit bien un type
					lpc_string_ptr = (lpc_string*)donnees;
					

					printf("%s\n",lpc_string_ptr->string);
					//on met à jour l'adresse à laquelle écrire les données suivantes
					donnees = donnees + sizeof(lpc_string) + lpc_string_ptr->slen; //ordre échangé par rapport à avant pour pouvoir récupérer slen et se décaler du bon nombre d'adresses avant d'effacer la mémoire
					//On efface la zone mémoire qui contenait ce lpc_string
					memset(lpc_string_ptr,0,sizeof(lpc_string) + lpc_string_ptr->slen);
					
					break;
				default:
					PANIC_EXIT("Unknown type\n");
					break;
			}

			type=va_arg(args2,lpc_type);	// type suivante dans la liste
		}
		va_end(args2);
	}else{ //traitement de l'erreur serveur
		//traiter_erreur_serveur(mem);
		printf("%s\n\n",strerror(mem->erreur));
		//S'il y a une erreur à cause d'une chaine de caractère trop courte pour recevoir le résultat de la fonction, on va chercher quelle est la chaine de caractère qui pose problème
		if(mem->erreur==ENOMEM){ 
					//Nous allons relire la liste des arguments pour connaîtres les types de ceux-ci
			va_list args2;
			va_start(args2,fun_name);
			type=va_arg(args2,lpc_type);
			
			donnees=(char*)mem + sizeof(memory) + sizeof(lpc_string) + fun_string->slen; //on replace donnees sur le premier des paramètres, ie après la structure memory et après le lpc string qui contient le nom de la fonction appelée
			int arg_counter=0,error_found=0;
			while(type!=NOP && error_found==0){ 
				arg_counter++;
				switch(type){
					case INT:
						int_ptr = va_arg(args2,int*);//il faut faire passer à l'arg suivant pour que le prochain soit bien un type
						int_ptr = (int*)donnees;

						//on met à jour l'adresse à laquelle lire les données suivantes
						donnees = donnees + sizeof(int);
						break;
						
					case DOUBLE:
						double_ptr = va_arg(args2,double*); //il faut faire passer à l'arg suivant pour que le prochain soit bien un type
						double_ptr = (double*)donnees;

						//on met à jour l'adresse à laquelle écrire les données suivantes
						donnees = donnees + sizeof(double);
						break;
					case STRING:
						lpc_string_ptr = va_arg(args2,lpc_string*); //il faut faire passer à l'arg suivant pour que le prochain soit bien un type
						lpc_string_ptr = (lpc_string*)donnees;
						//affichage seulement si c'est la chaine de caractère qui pose problème
						if(lpc_string_ptr->slen==-1){
							printf("La chaîne de caractère qui se trouve en position %d dans la liste des arguments est trop courte pour contenir la réponse\n",arg_counter);
							printf("La chaine de caractère en question est la suivante :\n");
							printf("%s\n",lpc_string_ptr->string);
							error_found=1;
						}
						
						//on met à jour l'adresse à laquelle écrire les données suivantes
						donnees = donnees + sizeof(lpc_string) + lpc_string_ptr->slen; //ordre échangé par rapport à avant pour pouvoir récupérer slen et se décaler du bon nombre d'adresses avant d'effacer la mémoire
						
						break;
					default:
						PANIC_EXIT("Unknown type\n");
						break;
				}

				type=va_arg(args2,lpc_type);	// taille suivante dans la liste
			}
		va_end(args2);
		}
		mem->erreur=0;
	}
	
	//################# fin de lecture des réponses ########################
	
	
	
	
	

	
	

	
	//on remet libre à 1
	mem->libre = 1; //les données ont été consommées
	
	//On déverrouille le mutex avant de finir
	code = pthread_mutex_unlock(&(mem->mutex));
	if( code > 0){
		thread_error( __FILE__ , __LINE__ , code, "mutex_unlock" );
	}




	
	return 0;
}





lpc_string *lpc_make_string(const char *s,int taille){
	if(taille>0 && s==NULL){
		printf("taille>0 et s=NULL");
		lpc_string *newString=malloc(sizeof(int)+(taille)*sizeof(char));
		newString->slen=taille;
		memset(newString->string,0,taille);
		return newString;
	}
	if(taille<=0 && s!= NULL){
			printf("taille<=0 et s!=NULL");
		lpc_string *newString=malloc(sizeof(int)+(strlen(s)+1)*sizeof(char));
		newString->slen=strlen(s)+1;
		memset(newString->string,0,strlen(s)+1);
		strcpy(newString->string,s);
		return newString;
	}
	if(taille >= (strlen(s)+1)){
		lpc_string *newString = malloc (sizeof(int)+(taille)*sizeof(char));
		newString->slen=taille;
		memset(newString->string,0,taille);
		strcpy(newString->string,s );
		return newString;
	}
	return NULL;
	
}
