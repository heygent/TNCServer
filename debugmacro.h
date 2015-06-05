//
// Created by bendetta on 05/06/15.
//

#ifndef PROGETTOSISTEMI_DEBUGMACRO_H
#define PROGETTOSISTEMI_DEBUGMACRO_H

#ifdef DEBUG
#define DBGPRINT(str) printf(str);
#else
#define DBGPRINT(str)
#endif

#define CONTROLLAERRORI(codice, messaggio) { 	\
	if(codice < 0) { 								\
		perror(messaggio); 						\
		exit(1); 									\
	} }


#endif //PROGETTOSISTEMI_DEBUGMACRO_H
