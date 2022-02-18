#ifndef COCA_H
#define COCA_H
#include <error.h>
#include <string.h>
#include "lpc_type.h"

int concat(void* param){ //concaténation de deux chaînes de caractères
	// la convention est que le résultat de la concaténation sera mise dans la première chaine de caractère
	lpc_string *str1=(lpc_string*)param; //on récupère la première chaine de caractères
	lpc_string *str2=(lpc_string*)( (char*)param + sizeof(lpc_string) + str1->slen  );
	//vérification des tailles i.e. vérifier si dans str1 on a assez de place pour stocker le résultat de la concaténation
	if( (strlen(str1->string) + strlen(str2->string) ) >= (str1->slen) ){ //s'il y a égalité dans le test, il n'y aura pas la place de stocker le caractère de fin de chaine de caractère
		str1->slen=-1; //cela signifie que c'est la chaien de caratère qui pose problème
		errno = ENOMEM;
		return -1;
	}else{
		//On fait la concaténation
		strncpy(str1->string + strlen(str1->string) , str2->string, strlen(str2->string)  );
		return 0;
	}
	
}
#endif
