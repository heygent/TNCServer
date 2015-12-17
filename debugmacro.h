#ifndef PROGETTOSISTEMI_DEBUGMACRO_H
#define PROGETTOSISTEMI_DEBUGMACRO_H
#include <stdlib.h>
#include <errno.h>


#define RUNTIME_ERROR(message) { \
    fprintf(stderr, "%s [%i]: %s", message, errno, strerror(errno)); \
    exit(EXIT_FAILURE); }

#endif //PROGETTOSISTEMI_DEBUGMACRO_H
