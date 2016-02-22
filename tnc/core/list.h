#ifndef TNC_LIST_H
#define TNC_LIST_H

#include "functiontypes.h"

/** @file 
 *  @brief Contiene l'interfaccia per le liste doppiamente concatenate.
 */

/** Lista doppiamente concatenata.
 *
 * La lista è definita come un puntatore opaco a una struttura.  Per
 * inizializzare una lista, si deve usare TNCList_new().
 *
 * Gli elementi della lista sono rappresentati internamente come puntatori a
 * void*, per cui è possibile aggiungere alla lista elementi di ogni tipo
 * (eccetto funzioni) e di diverso tipo tra loro.
 *
 * Gli elementi non vengono copiati prima dell'aggiunta alla lista.
 *
 * Le funzioni di rimozione prendono come parametro un puntatore a una funzione.
 * La funzione viene chiamata sull'elemento da rimuovere per eseguire operazioni
 * di pulizia.
 */
typedef struct _TNCList *TNCList;

/** Puntatore opaco al nodo di una lista.
 *
 * Puntatore a una struttura usata internamente per tenere traccia della
 * posizione del nodo.  Elementi di questo tipo vengono restituiti dalle
 * funzioni TNCList_next(), TNCList_previous(), TNCList_first() e
 * TNCList_last(), e possono essere usati per iterare attraverso la lista, per
 * indicare un nodo da rimuovere, o come punto di riferimento per inserire un
 * nuovo valore.  Si può ottenere l'elemento contenuto nel nodo con la funzione
 * TNCList_getvalue().
 *
 * @see TNCList_insert_before() @see TNCList_insert_after() @see
 * TNCList_remove()
 *
 */
typedef struct _TNCListNode *TNCListNode;

/** Inizializza una lista.
 *
 * @return Un puntatore a una lista inizializzata se l'allocazione va a buon
 * fine, NULL altrimenti.
 */
TNCList TNCList_new();

/** Restituisce il primo nodo di una lista.
 *
 * @param list La lista su cui si vuole operare.  Il comportamento della
 * funzione non è definito se questo parametro è NULL.
 *
 * @return Un TNCListNode corrispondente al primo nodo della lista, NULL se la
 * lista è vuota.
 *
 */
TNCListNode TNCList_first(TNCList list);

/** Restituisce l'ultimo nodo di una lista.
 *
 * @param list La lista su cui si vuole operare.  @paramnotnull
 *
 * @return Un TNCListNode corrispondente all'ultimo nodo della lista, NULL se la
 * lista è vuota.
 *
 * @see TNCListNode
 */
TNCListNode TNCList_last(TNCList list);

/** Restituisce il nodo successivo in una lista rispetto a un nodo passato come
 * parametro.
 *
 * @param current Il nodo di cui si vuole conoscere l'elemento successivo.  Il
 * parametro deve essere un nodo valido di una lista (non deve essere NULL e non
 * deve essere un nodo che è stato rimosso).
 *
 * @return Il nodo della lista successivo a quello passato come parametro.  NULL
 * se current è l'ultimo nodo della lista.
 *
 * @see TNCListNode
 */
TNCListNode TNCList_next(TNCListNode current);

/** Restituisce il nodo precedente in una lista rispetto a un nodo passato come
 * parametro.
 *
 * @param current Il nodo di cui si vuole conoscere l'elemento precedente.  Il
 * parametro deve essere un nodo valido di una lista (non deve essere NULL e non
 * deve essere un nodo che è stato rimosso).
 *
 * @return Il nodo della lista successivo a quello passato come parametro.  NULL
 * se current è il primo nodo della lista.
 *
 * @see TNCListNode
 */
TNCListNode TNCList_previous(TNCListNode current);


/** Inserisce un nuovo nodo come primo elemento della lista
 *
 * @param self La lista su cui si vuole operare.  Il comportamento della
 * funzione non è definito se questo parametro è NULL.
 *
 * @param val Il nuovo elemento da inserire nella lista.
 *
 * @return 0 se l'operazione va a buon fine, altrimenti uno dei seguenti codici
 * di errore: - TNCError_failed_alloc se non è stato possibile allocare memoria
 * per eseguire l'operazione.
 *
 * @see TNCListNode @see error.h
 */
int TNCList_push_front(TNCList restrict self, void *restrict val);

/** Inserisce un nuovo nodo come ultimo elemento della lista
 *
 * @param self La lista su cui si vuole operare.  Il comportamento della
 * funzione non è definito se questo parametro è NULL.
 *
 * @param val Elemento da inserire nella lista.
 *
 * @return 0 se l'operazione va a buon fine, altrimenti uno dei seguenti codici
 * di errore: - TNCError_failed_alloc se non è stato possibile allocare memoria
 * per eseguire l'operazione.
 */
int TNCList_push_back(TNCList restrict self, void *restrict val);

/** Inserisce un elemento prima del nodo passato come parametro.
 *
 * @param ref Il nodo di riferimento.  Il comportamento della funzione non è
 * definito se questo parametro è NULL.
 *
 * @param val Elemento da inserire nella lista
 *
 * @return 0 se l'operazione va a buon fine, altrimenti uno dei seguenti codici
 * di errore: - TNCError_failed_alloc se non è stato possibile allocare memoria
 * per eseguire l'operazione.
 *
 */

int TNCList_insert_before(TNCListNode restrict ref, void *restrict val);

/** Inserisce un elemento dopo il nodo passato come parametro.
 *
 * @param ref Il nodo di riferimento.  Il comportamento della funzione non è
 * definito se questo parametro è NULL.
 *
 * @param val Elemento da inserire nella lista
 *
 * @return 0 se l'operazione va a buon fine, altrimenti uno dei seguenti codici
 * di errore: - TNCError_failed_alloc se non è stato possibile allocare memoria
 * per eseguire l'operazione.
 *
 */
int TNCList_insert_after(TNCListNode restrict ref, void *restrict val);

/** Toglie il primo elemento dalla lista, e lo restituisce.
 *
 * @param self La lista su cui si vuole operare Il comportamento della funzione
 * non è definito se questo parametro è NULL o se la lista a cui si riferisce è
 * vuota.
 *
 * @return 0 se l'operazione va a buon fine, altrimenti uno dei seguenti codici
 * di errore: - TNCError_failed_alloc se non è stato possibile allocare memoria
 * per eseguire l'operazione.
 */
void *TNCList_pop_front(TNCList self);

/** Toglie l'ultimo elemento dalla lista, e lo restituisce.
 *
 * @param self La lista su cui si vuole operare.  Il comportamento della
 * funzione non è definito se questo parametro è NULL o se la lista a cui si
 * riferisce è vuota.
 *
 */
void *TNCList_pop_back(TNCList self);

/** Rimuove un elemento da una lista
 *
 * @param node Il nodo corrispondente all'elemento da togliere.  Se NULL, il
 * comportamento della funzione non è definito.
 *
 * @param freefn Una funzione da chiamare per dellocare l'elemento.  Se NULL,
 * non viene chiamata nessuna funzione sull'elemento e questo non viene
 * deallocato.
 *
 */
void *TNCList_remove(TNCListNode node);

/** Dato un nodo, restituisce un puntatore costante al suo valore.
 *
 * @param node Il nodo di cui si richiede il valore.  Il comportamento della
 * funzione è indefinito se questo parametro è NULL.
 *
 * @return Il valore corrispondente al nodo
 */
const void *TNCList_getvalue(TNCListNode node);

/** Cancella (dealloca) una lista.
 *
 * @param self La lista da cancellare
 */
void TNCList_destroy(TNCList self);

/** Cancella una lista e chiama una funzione su tutti i suoi elementi.
 *
 * @param self La lista da cancellare
 *
 * @param freefn Un puntatore a una funzione di deallocazione da chiamare per
 * ogni elemento nella lista.  Se NULL, nessuna funzione verrà chiamata e gli
 * elementi non verranno deallocati.
 *
 *
 */
void TNCList_destroy_and_free(TNCList self, TNCConsumer freefn);


/** Concatena due liste.
 *
 * @param list La lista con i primi elementi. Alla conclusione della funzione,
 * questa lista conterrà le due liste concatenate. Questo parametro non deve
 * essere NULL.
 *
 * @param to_chain La seconda lista da concatenare. Questa lista viene distrutta
 * al termine della funzione.  Questo parametro non deve essere NULL.
 *
 */
void TNCList_chain(TNCList restrict list, TNCList restrict to_chain);

/** Restituisce un valore diverso da zero se la lista è vuota, altrimenti 0.
 */
int TNCList_empty(TNCList self);

/** Restituisce il numero di elementi in una lista.
 */
size_t TNCList_length(TNCList self);

#endif //TNC_LIST_H
