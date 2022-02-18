#ifndef MEMORY_H
#define MEMORY_H
#include <pthread.h>
typedef struct{
	//HEADER
	pthread_mutex_t mutex;
	pthread_cond_t  ccond; /*variable condition pour le client */
	pthread_cond_t  scond; /* varioable condition pour le serveur */
	pid_t  pid;            // pid du dernier client qui a fait une requète, sera utile pour qu'un client qui se réveille sache la réponse en mémoire lui est destinée 
	int erreur;
	int retourFonction;
	int libre;             /* libre == 1 si le message a déjà été lu*/
	
	//DATA
	//Contiendra les données échangées entre le client et le serveur (bidirectionnel)
} memory;  
#endif

