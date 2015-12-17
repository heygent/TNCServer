//
// Created by root on 18/11/15.
//

#ifndef PROGETTOSISTEMI_PATHTOSERVE_H
#define PROGETTOSISTEMI_PATHTOSERVE_H

#include <stdio.h>

#define PATH_ROOT "/"
#define PATH_INDEXHTM "/index.htm"
#define PATH_INDEXHTML "/index.html"
#define PATH_DEFAULT_ERROR "./pages/%.3s.html"

extern const char *local_path_to_serve;
char *get_index_path();

#endif //PROGETTOSISTEMI_PATHTOSERVE_H