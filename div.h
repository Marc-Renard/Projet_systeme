#ifndef div_H
#define div_H
#include <error.h>
//#include "lpc_type.h"


int divise(void* param){ //param doit contenir 2 paramètres: deux int
	double *entier1=(double*)param; //on récupère la chaine de caractère
	double *entier2=(double*)param + 1;
	if((*entier2)==0){
		//division par 0
		errno = EDOM;
		return -1;
	}
	else{
		*entier1=(*entier1)/(*entier2);
		return 0; //Troncature réussie
	}
}
#endif
