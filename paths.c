#include "paths.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *get_index_path(TNCServer self)
{
    size_t local_path_len = strlen(self->local_path);
    char *index_path = malloc(local_path_len + strlen(PATH_INDEXHTML));

    strcpy(index_path, self->local_path);
    strcat(index_path, PATH_INDEXHTM);

    if(access(index_path, R_OK) == 0)
    {
        return index_path;
    }

    index_path[local_path_len] = '\0';
    strcat(index_path, PATH_INDEXHTML);

    return access(index_path, R_OK) == 0 ? index_path : NULL;

}
