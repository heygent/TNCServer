
class: center, middle

# TNC Server

## Un server HTTP/1.0

### Emanuele Gentiletti

---

class: center, middle

# Obiettivi

* Conformità alla RFC 1945
* Capacità di gestire richieste GET e HEAD
* File server
* Gestione carico con threadpool

---

# Convenzioni

* CMake è il sistema di build scelto.

* Le informazioni sugli oggetti che contengono comportamenti oltre che
informazioni vengono incapsulate tramite puntatori opachi.

* Elementi generali applicabili ad altri contesti oltre che a quello del server
(liste, threadpool) vengono compilati in una libreria statica, chiamata TNCCore
(target CMake tnccore).

* Ciò che è di competenza esclusiva del webserver è inserito nel target
TNCServer, ovvero l'eseguibile finale.


---

# Interfaccia principale

Questo è il puntatore alla struttura contenente le impostazioni del server, tra
cui la cartella da servire, la porta usata e il massimo numero di thread che il
server può usare.

```c
typedef struct _TNCServer *TNCServer;
```

La funzione `TNCServer_new()` crea una struttura di questo tipo e la configura
in base ai parametri passati:

```c
TNCServer TNCServer_new(
    const char *localpath, uint16_t door, size_t max_threads
);
```

---

# Interfaccia principale

Una volta ottenuta un'istanza di TNCServer, si può far iniziare a servire
richieste al server tramite `TNCServer_start()`

```c
int TNCServer_start(TNCServer self);
```

La chiamata è asincrona, il server viene eseguito in un nuovo thread.
Per terminare il server:

```c
void TNCServer_shutdown(TNCServer self, int flags)
```

Dove `flags` permette di scegliere se la chiamata debba bloccare o no fino al
termine dello shutdown del server (passando il flag
`TNCServer_shutdown_flags_async` la chiamata sarà asincrona)

---

# Funzionamento

Il programma principale (`main.c`) prende come parametri:
* Una stringa contenente il path in cui il server cercherà i file da servire.
* Un numero di porta opzionale. Se non specificata, la porta viene impostata a
  8080.

Il server risponderà alle richieste cercando i file nel path specificato.

---

# Funzionamento

Il server, alla ricezione di una richiesta:
* Apre una connessione con cui comunicare con il client.
* Fa il parsing della request line.
* Verifica che la risorsa esista e sia un file regolare.
  - Se non lo è, al posto della risorsa seleziona una pagina di errore.
* Legge informazioni sulla risorsa (dimensione, ultima modifica ecc.)
* Fa il parsing dei restanti header.
  - Supportati: Connection, If-Modified-Since
* Crea gli header di risposta
* Invia gli header di risposta e la risorsa, se necessario.



