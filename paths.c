//
// Created by root on 18/11/15.
//

#include "paths.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *localPathToServe = NULL;

char *getIndexPath()
{
    size_t localPathLen = strlen(localPathToServe);
    char *indexPath = calloc(localPathLen + strlen(PATH_INDEXHTML), 1);

    strcpy(indexPath, localPathToServe);
    strcat(indexPath, PATH_INDEXHTM);

    if(access(indexPath, R_OK) == 0)
    {
        return indexPath;
    }

    indexPath[localPathLen] = '\0';
    strcat(indexPath, PATH_INDEXHTML);

    if(access(indexPath, R_OK) == 0)
    {
        return indexPath;
    }
    
    return NULL;

}