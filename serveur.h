#ifndef SERVEUR_H
#define SERVEUR_H

#define NAMELEN 48




//gestion des fonctions client
#define NB_FONCTION 3  // à actualiser quand on ajoute une fonction
#include "tronc.h"
#include "concat.h"
#include "div.h"








lpc_function funTab[NB_FONCTION];

void initFunTab(){
	fptr f1 = &tronc;
	const lpc_function tronquer={"troncature",f1};
	funTab[0]=tronquer;
	fptr f2 = &concat;
	const lpc_function concatener={"concatener",f2};
	funTab[1]=concatener;
	fptr f3 = &divise;
	const lpc_function division={"diviser",f3};
	funTab[2]=division;
}


int (*findFuncPtr(const char *name)) (void*){ //Fonction qui prend en paramètre un const char( le nom de la fonction à chercher et qui retourne un pointeur de fonction qui elle renvoie un entier et prend un void* en paramètre
	for(int i = 0 ; i<NB_FONCTION ; i++){
		if(strcmp(funTab[i].fun_name,name)==0){ //comparaison de chaine de caractères
			return funTab[i].fun;
		}
	}
	return NULL; //Si on arrive ici, c'est que l'on n'a pas trouvé de fonction correspondante, on renvoit donc NULL
	//Ceci sera traité comme une erreur par le serveur
}


void *lpc_create(const char *mem_objet, size_t capacite) ;
int initialiser_mutex(pthread_mutex_t *pmutex);

#endif
