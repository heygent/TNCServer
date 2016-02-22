#ifndef TNC_TYPEDEFS_H
#define TNC_TYPEDEFS_H

/**
 * @file
 * @brief Contiene alias a tipo di puntatore a funzione.
 */

/** Alias a puntatore di funzione void(void*). */
typedef void (*TNCConsumer)(void *);

/** Alias a puntatore di funzione void*(void*). */
typedef void *(*TNCFunction)(void *);

#endif //TNC_TYPEDEFS_H
