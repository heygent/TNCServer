#ifndef TNC_PATHS_H
#define TNC_PATHS_H

#include <stdio.h>
#include "server.h"

/** @file
 * Contiene definizioni e funzioni utili nell'ambito dell'elaborazione dei
 * percorsi.
 */

#define PATH_ROOT "/"
#define PATH_INDEXHTM "/index.htm"
#define PATH_INDEXHTML "/index.html"
#define PATH_DEFAULT_ERROR "./pages/%i.html"

/** Dato un server, restituisce il percorso del file index della cartella
 * che serve.
 *
 * @param self Il server di riferimento.
 *
 * @return Il percorso di un file index.htm o index.html nella cartella che il
 * server sta servendo. Se non ce n'è uno, NULL. 
 *
 */
char *get_index_path(TNCServer self);

#endif //TNC_PATHS_H
