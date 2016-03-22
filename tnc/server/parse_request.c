#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include "server.h"
#include "httpdata.h"
#include "httpdate.h"


#define BUFFERSIZE 8096

const static char *error_page_format =
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
        "<meta charset=\"UTF-8\">\n"
        "<title>%s</title>\n" /* #STATCODE " " STATBRIEF */
    "</head>\n"
    "<body>\n"
        "<h1>%s</h1>\n" /* #STATCODE " " STATBRIEF */
        "<p>%s</p>\n" /* STATDESC */
    "</body>\n"
    "<!-- Sent by TNC Server -->\n"
    "</html>\n";

#define PATH_ROOT "/"
#define PATH_INDEXHTML "/index.html"

/** Comando da eseguire in una shell per ottenere una stringa contenente
 * il mimetype di un file. */

#define COMMAND_MIMETYPE "xdg-mime query filetype %s | tr '\\n' '\\0'"

/** Comando da eseguire in una shell per ottenere una stringa contenente la
 * codifica di un file. */

#define COMMAND_ENCODING "file -b --mime-encoding %s | tr '\\n' '\\0'"

/** @defgroup errorpages_x_macro
 *
 * Macro che crea funzioni di inizializzazione e di chiusura di file
 * temporanei in cui vengono inserite le pagine di errore utilizzate dal
 * server.
 * Le funzioni init_errorpage_##STATCODE, dove STATCODE è il codice di
 * errore HTTP, sono pensate per essere chiamate una sola volta nel corso
 * dell'esecuzione di un programma che usa il server tramite la funzione
 * pthread_once(), e registrano, tramite la funzione atexit(), la funzione
 * di chiusura corrispondente al file descriptor che inizializzano
 * (dichiarata dalla stessa macro con il nome close_errorpage_##STATCODE).
 * I file descriptor delle pagine di errore sono dichiarati staticamente
 * all'interno di questo file con il nome errorpage_fd_##STATCODE.
 * Le chiamate alla macro per ogni pagina di errore sono nel file
 * `httpstatus.x.h`
 *
 */
#define TNC_XM(STATCODE, STATBRIEF, STATDESC) \
static int errorpage_fd_##STATCODE = -1; \
static pthread_once_t once_control_##STATCODE = PTHREAD_ONCE_INIT; \
static void close_errorpage_##STATCODE() \
{ \
    close(errorpage_fd_##STATCODE); \
} \
static void init_errorpage_##STATCODE() \
{ \
    char errorpage_pattern[] = "/tmp/TNC_XXXXXX"; \
    errorpage_fd_##STATCODE = mkstemp(errorpage_pattern); \
    assert(errorpage_fd_##STATCODE != -1); \
    dprintf(errorpage_fd_##STATCODE, error_page_format, \
        #STATCODE " " STATBRIEF, \
        #STATCODE " " STATBRIEF, \
        STATDESC \
    ); \
    atexit(close_errorpage_##STATCODE); \
}

#include "httpstatus.x.h"

#undef TNC_XM

/** Esegue il parsing della request line.
 *
 * Esegue il parsing della request line, e inserisce l'esito dell'operazione in
 * request_data.
 */
static void parse_request_line(
    TNCServer self,
    HTTPRequestData *request_data,
    char *header
);

/** Esegue il parsing di un header.
 *
 * Esegue il parsing di un header di richiesta, e inserisce l'esito
 * dell'operazione in request_data.
 */
static void parse_header(
    TNCServer self,
    HTTPRequestData *request_data,
    HTTPRequestHeader *header
);

/** Verifica l'esistenza di un file `index.html` o `index.htm` all'interno
 * del path servito dal server. Restituisce una stringa allocata
 * dinamicamente contente il primo di questi che trova. Se non ne trova
 * nessuno, restituisce NULL.
 */
static char *get_index_path(TNCServer self)
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

/** Restituisce l'output di un comando eseguito in una shell.
 *
 * La funzione prende in input un comando da eseguire con al suo interno un
 * modificatore printf `%s`, che verrà sostituito con il secondo argomento,
 * `arg`.
 *
 * @param command Il comando di cui si vuole conoscere l'output
 *
 * @param arg L'argomento da sostituire all'interno della stringa del comando.
 *
 * @returns L'output fornito dalla shell.
 *
 */
static char *get_shell_output(const char *command, const char *arg)
{
    FILE *command_pipe;
    char *command_to_exec;

    char *result = malloc(256);
    size_t final_length;


    command_to_exec = malloc(
            strlen(arg) + strlen(command) + 1
    );

    sprintf(command_to_exec, command, arg);

    command_pipe = popen(command_to_exec, "r");
    fgets(result, 256, command_pipe);
    pclose(command_pipe);

    final_length = strlen(result) + 1;
    result = realloc(result, final_length);

    free(command_to_exec);
    return result;
}

void parse_request(TNCServer self,
                    char *request,
                    HTTPRequestData *request_data)
{

    request_data->status_code = 200;

    char *request_line;
    char *saveptr_line;

    struct stat fileinfo;


    request_line = strtok_r(request, "\n", &saveptr_line);
    parse_request_line(self, request_data, request_line);


    if(! (request_data->flags & HTTPRequestData_flags_get_error_page))
    {
        request_data->file_to_serve = open(request_data->file_info.path,
                                           O_RDONLY);

        if(-1 == request_data->file_to_serve)
        {
            request_data->status_code = 404;
            request_data->flags |= HTTPRequestData_flags_get_error_page;
        }
        else
        {
            fstat(request_data->file_to_serve, &fileinfo);

            if(S_ISREG(fileinfo.st_mode))
            {
                TNCJob *close_file =
                        TNCJob_new_close(request_data->file_to_serve);

                request_data->status_code = 200;
                TNCList_push_back(
                        request_data->cleanup_jobs, close_file
                );
            }
            else
            {
                request_data->status_code = 403;
                request_data->flags |= HTTPRequestData_flags_get_error_page;
            }
        }
    }

    if(request_data->flags & HTTPRequestData_flags_get_error_page)
    {
        switch (request_data->status_code)
        {

#define TNC_XM(STATCODE, STATBRIEF, STATDESC) \
        case STATCODE: \
            pthread_once(&once_control_##STATCODE, init_errorpage_##STATCODE); \
            request_data->file_to_serve = errorpage_fd_##STATCODE; \
            break;
#include "httpstatus.x.h"
#undef TNC_XM

        }
        fstat(request_data->file_to_serve, &fileinfo);
        request_data->file_info.mimetype = strdup("text/html");
        request_data->file_info.encoding = strdup("UTF-8");
    }
    else
    {
        request_data->file_info.mimetype =
            get_shell_output(COMMAND_MIMETYPE, request_data->file_info.path);

        request_data->file_info.encoding =
            get_shell_output(COMMAND_ENCODING, request_data->file_info.path);
    }

    TNCList_push_back(
            request_data->cleanup_jobs,
            TNCJob_new_free(request_data->file_info.mimetype)
    );

    TNCList_push_back(
            request_data->cleanup_jobs,
            TNCJob_new_free(request_data->file_info.encoding)
    );

    request_data->file_info.last_edit = fileinfo.st_mtime;
    request_data->file_info.size = (size_t) fileinfo.st_size;

    while(! (request_data->flags & HTTPRequestData_flags_stop_parsing))
    {
        HTTPRequestHeader header;

        request_line = strtok_r(NULL, CRLF, &saveptr_line);
        if(!request_line) break;

        header.field_name = strsep(&request_line, ":");
        if(!header.field_name) continue;

        header.field_content = request_line;
        if(!header.field_content) continue;

        while(isspace(*header.field_content))
            ++header.field_content;

        parse_header(self, request_data, &header);
    }

    return;

}


static void parse_request_line(
    TNCServer self,
    HTTPRequestData *request_data,
    char *header
)
{

    char *current_token, *saveptr_space;
    char *path_to_serve;
    const char *localpath;
    enum HTTPMethod method;

    localpath = TNCServer_getlocalpath(self);

    current_token = strtok_r(header, " ", &saveptr_space);

    // Leggi il metodo HTTP

    if (!current_token) goto bad_request;
    else if (strcasecmp(current_token, "GET") == 0) method = HTTPMethod_GET;
    else if (strcasecmp(current_token, "HEAD") == 0) method = HTTPMethod_HEAD;
    else method = HTTPMethod_unrecognized;

    request_data->method = method;

    // Leggi il percorso richiesto

    current_token = strtok_r(NULL, " ", &saveptr_space);

    if (!current_token) goto bad_request;
    request_data->remote_path = strdup(current_token);

    TNCList_push_back(
            request_data->cleanup_jobs,
            TNCJob_new_free(request_data->remote_path)
    );

    // Leggi versione HTTP

    current_token = strtok_r(NULL, " ", &saveptr_space);


    if(!current_token)
    {
        request_data->http_version_major = 0;
        request_data->http_version_minor = 9;
    }

    else if (strncmp("HTTP/1.0", current_token, sizeof("HTTP/1.0") - 1) == 0)
    {
        request_data->http_version_major = 1;
        request_data->http_version_minor = 0;
    }
    else if (strncmp("HTTP/1.1", current_token, sizeof("HTTP/1.1") - 1) == 0)
    {
        request_data->http_version_major = 1;
        request_data->http_version_minor = 1;
    }
    else goto bad_request;

    if (current_token) current_token = strtok_r(NULL, " ", &saveptr_space);
    if (current_token) goto bad_request;

    // Ottieni informazioni sul file richiesto

    switch(request_data->method)
    {
    case HTTPMethod_HEAD:
        request_data->flags |= HTTPRequestData_flags_dont_send_payload;
        break;

    case HTTPMethod_GET:
        break;

    default:
        request_data->status_code = 501;

        request_data->flags |= HTTPRequestData_flags_stop_parsing;
        request_data->flags |= HTTPRequestData_flags_get_error_page;

        return;
    }

    if (strcmp(request_data->remote_path, PATH_ROOT) == 0)
    {
        path_to_serve = get_index_path(self);
    }
    else
    {
        path_to_serve = malloc(strlen(localpath) +
                               strlen(request_data->remote_path) + 1);

        strcpy(path_to_serve, localpath);
        strcat(path_to_serve, request_data->remote_path);
    }

    request_data->file_info.path = path_to_serve;

    TNCList_push_back(
            request_data->cleanup_jobs,
            TNCJob_new_free(request_data->file_info.path)
    );

    return;

bad_request:

    request_data->status_code = 400;
    request_data->flags |= HTTPRequestData_flags_stop_parsing;
    request_data->flags |= HTTPRequestData_flags_get_error_page;
    request_data->flags |= HTTPRequestData_flags_bad_request;

    request_data->http_version_major = 1;
    request_data->http_version_minor = 0;

    return;
}

static void parse_header(
    TNCServer self,
    HTTPRequestData *request_data,
    HTTPRequestHeader *header
)
{
    if(strcmp(header->field_name, "Connection") == 0)
    {
        if(strcasecmp(header->field_content, "keep-alive") == 0)
        {
            request_data->flags |= HTTPRequestData_flags_keep_alive;
        }
        return;
    }

    if(strcmp(header->field_name, "If-Modified-Since") == 0)
    {
        if(request_data->status_code != 200) return;

        struct tm if_modified_since = { 0 };
        char *date_parse_last_char;
        date_parse_last_char = strptime_httpdate(
                                   header->field_content, &if_modified_since);

        if(date_parse_last_char != NULL && *date_parse_last_char == '\0')
        {
            time_t modified_since = mktime(&if_modified_since);

            /* If the date provided is less or equal than date of last edit */
            if(modified_since <= request_data->file_info.last_edit)
            {
                request_data->status_code = 304;
                request_data->flags |= HTTPRequestData_flags_dont_send_payload;

                return;
            }
        }

        return;
    }
}
