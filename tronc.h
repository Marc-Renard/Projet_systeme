#ifndef TRONC_H
#define TRONC_H
#include <error.h>
#include "lpc_type.h"
#include <string.h>

int tronc(void* param){ //param doit contenir 2 paramètres: un lpc_string et une taille pour tronquer cette lpc_string à cette taille
	lpc_string *str=(lpc_string*)param; //on récupère la chaine de caractère
	int *tailleFinale=(int*)( (char*)param + sizeof(lpc_string) + str->slen); //On se place sur le deuxième paramètre
	if((*tailleFinale)>=str->slen){
		// Echec de la troncature, le texte initial est déjà plus petit que la taille finale 
		*tailleFinale=0; //On se sert de ce deuxième paramètre pour indiquer le nombre de caractères supprimés par la troncature
		errno = EINVAL;
		return -1;
	}
	else{
		//str->string[*tailleFinale]=0;
		char tmp[str->slen];
		memcpy(tmp,str->string,str->slen); //On garde uen copie de la chaine de caractère originale
		memset(str->string,0,str->slen); //On supprime toute la chaine de caractère dans le lpc_string
		memcpy(str->string,tmp,*tailleFinale); //et on y recopie uniquement tailleFinale caractères de la chaine originale
		*tailleFinale = strlen(tmp) - (*tailleFinale); //taille d'origine - taille finale
		//en plaçant un 0 dans la case d'indice taille finale, cela "coupe" la chaine de caractère à la bonne taille
		return 0; //Troncature réussie
	}
}
#endif
