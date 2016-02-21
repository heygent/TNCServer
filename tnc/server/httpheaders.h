#ifndef TNC_HTTPHEADERS_H
#define TNC_HTTPHEADERS_H

#define HEADERSIZE 8096
#define CRLF "\r\n"

#define HTTPDATE "%a, %d %b %Y %T GMT"

#define HTTP10 "HTTP/1.0"
#define HTTP11 "HTTP/1.1"


// Response Header
#define RHDR_STATUSLINE_OK                "200 OK"
#define RHDR_STATUSLINE_NOTMODIFIED       "304 Not Modified"
#define RHDR_STATUSLINE_BADREQUEST        "400 Bad Request"
#define RHDR_STATUSLINE_NOTFOUND          "404 Not Found"
#define RHDR_STATUSLINE_NOTIMPLEMENTED    "501 Not Implemented"


#define RHDR_CONNECTION             "Connection: %s"
#define RHDR_CONTENT_TYPE           "Content-Type: %s; charset=%s"
#define RHDR_CONTENT_LENGTH         "Content-Length: %s"
#define RHDR_LAST_MODIFIED          "Last-Modified: " HTTPDATE 
#define RHDR_DATE                   "Date: " HTTPDATE 

#define GETSTR "GET"
#define HEADSTR "HEAD"
#define POSTSTR "POST"


enum HTTPMethod {
    HTTPMethod_unrecognized, HTTPMethod_GET, HTTPMethod_HEAD
};

#define COMMAND_MIMETYPE "xdg-mime query filetype %s | tr '\\n' '\\0'"
#define COMMAND_ENCODING "file -b --mime-encoding %s | tr '\\n' '\\0'"



#endif //TNC_HTTPHEADERS_H
