#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "paths.h"
#include "server.h"

char *get_index_path(TNCServer self)
{
    const char *localpath = TNCServer_getlocalpath(self);
    size_t local_path_len = strlen(localpath) + sizeof(PATH_INDEXHTML);
    char *index_path = malloc(local_path_len);

    strcpy(index_path, localpath);
    strcat(index_path, PATH_INDEXHTML);

    if(access(index_path, R_OK) == 0)
    {
        return index_path;
    }

    index_path[local_path_len - 2] = '\0';

    if(access(index_path, R_OK) == 0)
      return index_path;
    else
    {
      free(index_path);
      return NULL;
    }
}
