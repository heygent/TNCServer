#define _XOPEN_SOURCE
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "httpdate.h"

char *strptime_httpdate(const char *s, struct tm *tm)
{
    if(!s || strlen(s) < 4) return NULL;

    char defining_char = *(s + 3);

    if(defining_char == ',')
    {
        return strptime(s, HTTPDATE_RFC1123, tm);
    }
    else if(isalpha(defining_char))
    {
        return strptime(s, HTTPDATE_RFC850, tm);
    }
    else if(defining_char == ' ')
    {
        return strptime(s, HTTPDATE_ASCTIME, tm);
    }
    else
    {
        return NULL;
    }
}
