#ifndef TNC_HTTPDATE_H
#define TNC_HTTPDATE_H

/** @file
 * Funzioni e definizioni utili alla creazione e alla manipolazione delle
 * date nei formati supportati da HTTP.
 */

/** @name HTTPDATE
 * Le definizioni contengono le stringhe in formato leggibile a strptime()
 * rappresentanti le date supportate in ricezione dal server, come indicate
 * dalla RFC 1945. 
 *
 * @{
 */

#define HTTPDATE_RFC1123 "%a, %d %b %Y %T GMT"
#define HTTPDATE_RFC850  "%A, %d-%b-%y %T GMT"
#define HTTPDATE_ASCTIME "%a %b %d %T %Y"

#define HTTPDATE HTTPDATE_RFC1123

/** @} */

/** Riempie una struct tm a seconda delle informazioni contenute in una stringa.
 * La funzione si comporta in modo analogo alla funzione POSIX strptime(), ma
 * specificamente per le date HTTP. Il protocollo HTTP/1.0 accetta tre diversi
 * formati per la data; la funzione si occupa di scegliere il formato
 * appropriato per la stringa ricevuta come parametro, di eseguire strptime()
 * su di questa e di restituirne il risultato.
 *
 * @param date La stringa contentente la data di cui fare il parsing.
 *
 * @param tm La struttura da riempire con le informazioni sulla data.
 *
 * @see strptime()
 */
char *strptime_httpdate(const char *date, struct tm *tm);

#endif //TNC_HTTPDATE_H
