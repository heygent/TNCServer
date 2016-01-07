#ifndef TNC_HTTPHEADERS_H
#define TNC_HTTPHEADERS_H

#define HEADERSIZE 8096

// https://tools.ietf.org/html/rfc7231#section-7.1.1.1
#define HTTPDATE "%a, %d %b %Y %T GMT"

#define HTTP10 "HTTP/1.0"
#define HTTP11 "HTTP/1.1"


// Response Header
#define RHDR_MAIN_OK                "200 OK"
#define RHDR_MAIN_BADREQUEST        "400 Bad Request"
#define RHDR_MAIN_NOTFOUND          "404 Not Found"
#define RHDR_MAIN_NOTIMPLEMENTED    "501 Not Implemented"


#define RHDR_CONNECTION             "Connection: %s"
#define RHDR_CONTENT_TYPE           "Content-Type: %s; charset=%s"
#define RHDR_CONTENT_LENGTH         "Content-Length: %s"
#define RHDR_LAST_MODIFIED          "Last-Modified: " HTTPDATE 
#define RHDR_DATE                   "Date: " HTTPDATE 

#define GETSTR "GET"
#define HEADSTR "HEAD"
#define POSTSTR "POST"

#define M_GET 0
#define M_HEAD 1
#define M_POST 2
#define M_BAD 255

enum HTTPMethod {
    HTTP_METHOD_GET, HTTP_METHOD_HEAD, HTTP_METHOD_POST
};

#define COMMAND_MIMETYPE "xdg-mime query filetype %s | tr '\\n' '\\0'"
#define COMMAND_ENCODING "file -b --mime-encoding %s | tr '\\n' '\\0'"


#endif //TNC_HTTPHEADERS_H