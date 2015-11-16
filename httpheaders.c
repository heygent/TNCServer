//
// Created by jeff on 15/11/15.
//
#include "httpheaders.h"

const contenitoreHeader HEADERS[] =
{
    { 200, "HTTP/1.1 200 OK\n" },
    { 401, "HTTP/1.1 401 Unauthorized\n" },
    { 402, "HTTP/1.1 402 Bad Request\n" },
    { 403, "HTTP/1.1 403 Forbidden\n" },
    { 404, "HTTP/1.1 404 Not Found\n" },
    { 405, "HTTP/1.1 405 Method Not Allowed\nAllowed: GET, HEAD\n" }
};