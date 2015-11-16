#ifndef PROGETTOSISTEMI_DEBUGMACRO_H
#define PROGETTOSISTEMI_DEBUGMACRO_H
#include <stdlib.h>
#include <errno.h>

#ifdef DEBUG
#define DBGPRINT(str) printf(str);
#else
#define DBGPRINT(str)
#endif

#define CONTROLLAERRORI(codice, messaggio) { 	\
	if(codice < 0) { 								\
		fprintf(stderr, "%s [%i]: %s", messaggio, errno, strerror(errno)); 						\
		exit(1); 									\
	} }


#endif //PROGETTOSISTEMI_DEBUGMACRO_H
