#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>

#include "httpheaders.h"

headerLine *makeContentHeaders(headerLine *toChain, requestInfo *info)
{

    FILE *pipeToCommand;
    char *fileCommand;

    char mimetype[256];
    char encoding[256];
    char filesize[32];
    struct tm *lastModified;

    char *pathToServe;
    char *headerStr;
    size_t hdrLen;

    headerLine *nextHdr;

    pathToServe = info->servedPath;

    // Content type header

    nextHdr = calloc(1, sizeof(headerLine));
    bzero(&mimetype, 256);
    bzero(&encoding, 256);
    bzero(&filesize, 32);

    toChain->nextLine = nextHdr;

    // get mimetype

    fileCommand = calloc(strlen(pathToServe) + strlen(COMMAND_MIMETYPE), 1);
    sprintf(fileCommand, COMMAND_MIMETYPE, pathToServe);

    pipeToCommand = popen(fileCommand, "r");
    fgets(mimetype, 256, pipeToCommand);
    pclose(pipeToCommand);

    free(fileCommand);

    // get encoding

    fileCommand = malloc(strlen(pathToServe) + strlen(COMMAND_ENCODING));
    sprintf(fileCommand, COMMAND_ENCODING, pathToServe);

    pipeToCommand = popen(fileCommand, "r");
    fgets(encoding, 256, pipeToCommand);
    pclose(pipeToCommand);

    free(fileCommand);

    // print to header string

    hdrLen = strlen(RHDR_CONTENT_TYPE) + strlen(mimetype) + strlen(encoding) + 1;
    headerStr = malloc(hdrLen);
    sprintf(headerStr, RHDR_CONTENT_TYPE, mimetype, encoding);

    nextHdr->string = headerStr;
    nextHdr->nextLine = calloc(1, sizeof(headerLine));
    nextHdr = nextHdr->nextLine;

    // Content length header

    sprintf(filesize, "%jd", info->fileinfo->st_size);

    hdrLen = strlen(RHDR_CONTENT_LENGTH) + strlen(filesize) + 1;
    headerStr = malloc(hdrLen);
    sprintf(headerStr, RHDR_CONTENT_LENGTH, filesize);

    nextHdr->string = headerStr;
    nextHdr->nextLine = malloc(sizeof(headerLine));
    nextHdr = nextHdr->nextLine;

    // Last Modified Header

    hdrLen = strlen(RHDR_CONTENT_LENGTH) + 32;
    headerStr = malloc(hdrLen);
    lastModified = gmtime(&info->fileinfo->st_mtime);
    strftime(headerStr, hdrLen, RHDR_LAST_MODIFIED, lastModified);


    nextHdr->string = headerStr;
    return nextHdr;

}

headerLine *makeDateHeader(headerLine *toChain)
{
    headerLine *dateHeader;
    char *headerLine;
    size_t headerLen;

    time_t rawTime;
    struct tm *timeinfo;

    time(&rawTime);
    timeinfo = gmtime(&rawTime);

    dateHeader = malloc(sizeof(headerLine));
    headerLen = strlen(RHDR_DATE) + 32;
    headerLine = calloc(1, headerLen);
    strftime(headerLine, headerLen, RHDR_DATE, timeinfo);

    dateHeader->string = headerLine;
    toChain->nextLine = dateHeader;
    return dateHeader;
}


void freeRequestInfo(requestInfo *tofree)
{
    free(tofree->fileinfo);
    free(tofree->requestedPath);

    while(tofree->firstHeaderLine != NULL)
    {
        headerLine *next = tofree->firstHeaderLine->nextLine;
        free(tofree->firstHeaderLine);
        tofree->firstHeaderLine = next;
    }

    fclose(tofree->toSend);

    free(tofree);
    return;
}