#ifndef PROGETTOSISTEMI_HTTPDATE_H
#define PROGETTOSISTEMI_HTTPDATE_H


/*     date1          = 2DIGIT SP month SP 4DIGIT
                        ; day month year (e.g., 02 Jun 1982)
       date2          = 2DIGIT "-" month "-" 2DIGIT
                        ; day-month-year (e.g., 02-Jun-82)
       date3          = month SP ( 2DIGIT | ( SP 1DIGIT ))
                        ; month day (e.g., Jun  2)
 *
 *
 *     rfc1123-date   = wkday "," SP date1 SP time SP "GMT"
 *     rfc850-date    = weekday "," SP date2 SP time SP "GMT"
 *     asctime-date   = wkday SP date3 SP time SP 4DIGIT
 *
 *     Sun, 06 Nov 1994 08:49:37 GMT    ; RFC 822, updated by RFC 1123
       Sunday, 06-Nov-94 08:49:37 GMT   ; RFC 850, obsoleted by RFC 1036
       Sun Nov  6 08:49:37 1994         ; ANSI C's asctime() format
 */

#define HTTPDATE_RFC1123 "%a, %d %b %Y %T GMT"
#define HTTPDATE_RFC850  "%A, %d-%b-%y %T GMT"
#define HTTPDATE_ASCTIME "%a %b %d %T %Y"

char *strptime_httpdate(const char *date, struct tm *tm);

#endif //PROGETTOSISTEMI_HTTPDATE_H
