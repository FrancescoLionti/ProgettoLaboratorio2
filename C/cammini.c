#include "xerrori.h"
#include "supportoBFS.h"
#define QUI __LINE__, __FILE__
#include <sys/times.h>

// Struttura usata per la gestione dei segnali
typedef struct
{
    int faseLettura;        // indica se la fase di lettura è iniziata
    int termina;            // indica al main che deve terminare
    pthread_mutex_t *mutex; // mutex per sincronizzare l'accesso al file
} datiGestoreSegnali;

// Struttura usata per passare i dati al thread calcolatore
typedef struct
{
    int a; // sorgente
    int b; // destinazione
} coppia;

// Struttura che rappresenta un attore
typedef struct
{
    int codice; // codice attore
    char *nome; // nome attore
    int anno;   // anno di nascita
    int numcop; // numero coprotagonisti
    int *cop;   // array coprotagonisti
} attore;

// Struttura che rappresenta il thread consumatore
typedef struct
{
    char **buffer;          // buffer condiviso tra i threads
    int dimBuffer;          // dimensione del buffer
    attore *attori;         // array di attori
    int numAttori;          // numero di attori presenti dentro attori
    int *pIndex;            // indice del produttore
    int *cIndex;            // indice dei consumatori
    sem_t *slots_liberi;    // numero di slots liberi
    sem_t *slots_occupati;  // numero di slots occupati
    pthread_mutex_t *mutex; // mutex che gestisce l'accesso al buffer

} consum;

// Rappresenta il thread che calcola il cammino minimo tra a ed b
typedef struct
{
    int a;            // sorgente
    int b;            // destinazione
    int numeroAttori; // numeroAttori mi servirà per bsearch
    attore *grafo;    // il grafo (cioè l'array di attori)
} calcolatore;

// Funzione di comparazione per bsearch
int comparaCodici(const void *a, const void *b)
{
    int codiceA = *((int *)a);           // Tratto 'a' come un intero (chiave di ricerca)
    int codiceB = ((attore *)b)->codice; // 'b' è un puntatore a 'attore', quindi prendo il suo 'codice'

    if (codiceA < codiceB)
        return -1;
    if (codiceA > codiceB)
        return 1;
    return 0;
}

// Funzione che calcola in numero di attori contenuti in nomi.txt
int contaAttori(const char *nomitxt)
{

    char *linea = NULL; // serve per lettura di nomi.txt
    size_t dimLinea = 0;
    int conta = 0;

    FILE *nomiFile = xfopen(nomitxt, "r", QUI);

    // scorro tutto il file nomi.txt per contare il numero di attori
    while (getline(&linea, &dimLinea, nomiFile) != -1)
        conta += 1;

    if (ferror(nomiFile))
        xtermina("Errore durante la lettura del file nomi.txt", QUI);

    free(linea);      // Libera la memoria di getline
    fclose(nomiFile); // Chiudo il file

    return conta;
}

// Funzione che inizializza i campi codice,nome,anno di ogni attore
void inizializzaAttori(const char *nomitxt, attore *attori)
{
    char *linea = NULL; // serve per lettura di nomi.txt
    size_t dimLinea = 0;
    int posizione = 0; // mi serve dopo per inizializzare gli attori

    FILE *nomiFile = xfopen(nomitxt, "r", QUI);
    // inizializzo gli attori con i dati contenuti in nomi.txt
    while (getline(&linea, &dimLinea, nomiFile) != -1)
    {
        char **dati = malloc(3 * sizeof(char *)); // conterrà temporaneamente i "dati": codice,nome,anno di ogni attore
        int i = 0;
        char *dato = strtok(linea, "\t");

        while (dato != NULL)
        {
            dati[i++] = dato;
            dato = strtok(NULL, "\t");
        }
        // inizializzo ogni attore con i propi dati
        attori[posizione].codice = atoi(dati[0]);
        attori[posizione].anno = atoi(dati[2]);

        char *copiaNome = strdup(dati[1]);
        if (copiaNome == NULL)
            xtermina("C'è stato un errore nella creazione del nome", QUI);
        attori[posizione].nome = copiaNome;

        posizione++;
        free(dati);
    }

    if (ferror(nomiFile))
        xtermina("Errore durante la lettura del file nomi.txt", QUI);

    if (linea == NULL)
        xtermina("C'è stato un errore nella creazione della linea", QUI);

    free(linea);      // Libera la memoria di getline
    fclose(nomiFile); // chiudo il file nomi.txt
}

// Funzione che viene eseguita dai threads consumatori
void *consumatoreBody(void *args)
{
    consum *c = (consum *)args;
    char *stato = NULL; // stato per strtok_r, anche se i thread accedono a stringhe diverse, per sicurezza uso la versione thread-safe
    while (1)
    {
        xsem_wait(c->slots_occupati, QUI); // se non c'è nulla da consumare aspetto
        xpthread_mutex_lock(c->mutex, QUI);
        char *riga = c->buffer[(*c->cIndex)++ % c->dimBuffer]; // prendo la riga dal buffer circolare, e poi aggiorno l'indice dei consumatori
        xpthread_mutex_unlock(c->mutex, QUI);

        if (riga == NULL)
        {
            // notifico al produttore che ho terminato
            xsem_post(c->slots_liberi, QUI);
            break;
        }

        // Rimuovo caratteri di fine riga
        riga[strcspn(riga, "\n")] = 0;
        riga[strcspn(riga, "\r")] = 0;

        // Parsing del codice attore
        char *codiceStr = strtok_r(riga, "\t", &stato);
        if (codiceStr == NULL)
            xtermina("!!Errore: Codice attore mancante o malformato\n ", QUI);
        int codiceAttore = atoi(codiceStr);

        // Parsing numero Coprotagonisti
        char *coprotagonistiStr = strtok_r(NULL, "\t", &stato);
        if (coprotagonistiStr == NULL)
            xtermina("!!Errore: CoprotagonistiStr mancante o malformato\n ", QUI);

        int numCoprotagonisti = atoi(coprotagonistiStr);
        if (numCoprotagonisti < 0)
            xtermina("!!Errore: NumCoprotagonisti è negativo, ma deve essere positivo\n ", QUI);

        // prendo l'attore dall'array attori
        attore *att = bsearch(&codiceAttore, c->attori, c->numAttori, sizeof(attore), comparaCodici);
        if (att == NULL)
            xtermina("!!Errore: L'attore non è stato trovato\n ", QUI);

        // inizializzo i campi cop e numcop dell'attore
        att->numcop = numCoprotagonisti;
        att->cop = malloc(numCoprotagonisti * sizeof(int));
        if (att->cop == NULL)
            xtermina("Errore di allocazione array coprotagonisti", QUI);

        // riempio cop con i coprotagonisti
        for (int i = 0; i < numCoprotagonisti; i++)
        {
            char *codiceCop = strtok_r(NULL, "\t", &stato);
            if (codiceCop == NULL)
                xtermina("Errore: meno protagonisti del previsto", QUI);

            att->cop[i] = atoi(codiceCop);
        }

        free(riga);                      // Libera la memoria della riga dopo averla processata
        xsem_post(c->slots_liberi, QUI); // segnalo che ho liberato uno slot
    }
    return NULL;
}

// Il produttore che riempie il buffer,chiama i consumatori ed quando ha/hanno concluso li termina
void produttore(const char *grafotxt, int numConsumatori, int numeroAttori, attore *attori)
{
    char *linea = NULL; // serve per lettura di nomi.txt e grafo.txt
    size_t dimLinea = 0;
    // creazione del buffer condiviso tra i threads
    int dimBuffer = 2 * numConsumatori;                 // La dimensione del buffer condiviso, sarà pari al doppio del numero dei consumatori, in modo da ottimizzare l'utilizzo
    char **buffer = malloc(dimBuffer * sizeof(char *)); // buffer di puntatori a stringhe
    if (buffer == NULL)
        xtermina("C'è stato un errore nella creazione del buffer", QUI);

    int pIndex = 0; // Indice del produttore
    int cIndex = 0; // Indice dei consumatori

    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // mutex per sincronizzare i threads

    pthread_t *t = malloc(numConsumatori * sizeof(pthread_t)); // contiene i threads consumatori
    consum *tcb = malloc(numConsumatori * sizeof(consum));     // contiene le struct che definiscono i rispettivi threads consumatori

    sem_t slots_liberi, slots_occupati; // semafori che tengono traccia degli slots liberi e occupati
    xsem_init(&slots_liberi, 0, dimBuffer, QUI);
    xsem_init(&slots_occupati, 0, 0, QUI);

    // inizializzo i threads consumatori
    for (size_t i = 0; i < numConsumatori; i++)
    {
        tcb[i].attori = attori;
        tcb[i].buffer = buffer;
        tcb[i].dimBuffer = dimBuffer;
        tcb[i].numAttori = numeroAttori;
        tcb[i].slots_liberi = &slots_liberi;
        tcb[i].slots_occupati = &slots_occupati;
        tcb[i].cIndex = &cIndex;
        tcb[i].pIndex = &pIndex;
        tcb[i].mutex = &mutex;

        xpthread_create(&t[i], NULL, consumatoreBody, &tcb[i], QUI); // creazione del thread consumatore
    }

    // Threads principale (Produttore)
    FILE *grafoFile = xfopen(grafotxt, "r", QUI);
    while (getline(&linea, &dimLinea, grafoFile) != -1)
    {
        char *riga = strdup(linea); // viene allocata memoria per la riga, e deallocata quando il consumatore la consuma
        if (riga == NULL)
            xtermina("Errore nella duplicazione della riga", QUI);

        free(linea);  // Libera la memoria di getline
        linea = NULL; // Resetto per la prossima getline

        xsem_wait(&slots_liberi, QUI); // se non ci sono slots liberi aspetto
        xpthread_mutex_lock(&mutex, QUI);
        buffer[(pIndex++) % dimBuffer] = riga; // inserisco il puntatore alla stringa nel buffer circolare
        xpthread_mutex_unlock(&mutex, QUI);
        xsem_post(&slots_occupati, QUI); // incremento il numero di slots occupati
    }

    if (ferror(grafoFile))
        xtermina("Errore durante la lettura del file grafo.txt", QUI);

    fclose(grafoFile);

    // nofico la terminazione ai threads consumatori
    for (size_t i = 0; i < numConsumatori; i++)
    {
        xsem_wait(&slots_liberi, QUI); // se non ci sono slots liberi aspetto
        xpthread_mutex_lock(&mutex, QUI);
        buffer[(pIndex++) % dimBuffer] = NULL; // inserisco NULL nel buffer circolare, per far capire ai consumatori che devono terminare
        xpthread_mutex_unlock(&mutex, QUI);
        xsem_post(&slots_occupati, QUI); // incremento il numero di slots occupati
    }

    // aspetto che tutti i consumatori abbiano terminato
    for (size_t i = 0; i < numConsumatori; i++)
        xpthread_join(t[i], NULL, QUI);

    // Libero la memoria da tutte le risorse che non mi servono più
    free(linea);
    free(t);
    free(tcb);
    sem_destroy(&slots_liberi);
    sem_destroy(&slots_occupati);
    pthread_mutex_destroy(&mutex);
    free(buffer);
}

// Funzione che scrive i cammini nel file a.b e stampa a video il messaggio
void scriviCammini(int a, int b, nodoAbr *alberoCopertura, int trovato, double tempo)
{

    char *strCammino = NULL;
    if (asprintf(&strCammino, "%d.%d", a, b) == -1)
        xtermina("Errore nella creazione della stringa", QUI);

    FILE *camminiFile = xfopen(strCammino, "w", QUI);

    switch (trovato)
    {
    case -1: // se la sorgente non appartiene al grafo
        fprintf(camminiFile, "codice %d non valido\n", a);
        printf("%s: Codice %d non valido. Tempo di elaborazione:%f\n", strCammino, a, tempo);
        break;
    case -2: // se lad destinazione non appartiene al grafo
        fprintf(camminiFile, "codice %d non valido\n", b);
        printf("%s: Codice %d non valido. Tempo di elaborazione:%f\n", strCammino, b, tempo);
        break;
    case 0: // se il cammino non esiste
        fprintf(camminiFile, "non esistono cammini da %d a %d\n", a, b);
        printf("%s: Nessun cammino. Tempo di elaborazione:%f \n", strCammino, tempo);
        break;

    default: // se il cammino esiste, scrivo i cammini nel file e stampo a video il mesaggio
        nodoAbr *nodoCercato = cerca(alberoCopertura, shuffle(b));
        int profondita = nodoCercato->profondita;
        if (profondita < 0)
            xtermina("Errore: La profondità non deve essere negativa)", QUI);

        // array di nodi che fanno parte del cammino
        nodoAbr **cammino = malloc((profondita + 1) * sizeof(nodoAbr *));
        if (!cammino)
            xtermina("Errore: allocazione cammino fallita", QUI);

        // riempio l'array di nodi che fanno parte del cammino
        ricostruisciCammino(alberoCopertura, a, b, cammino);

        // scrivo i cammini nel file
        for (size_t i = 0; i < (profondita + 1); i++)
        {
            fprintf(camminiFile, "%d\t%s\t%d\n", unshuffle(cammino[i]->val), cammino[i]->nome, cammino[i]->anno);
        }

        printf("%s: Lunghezza minima: %d, tempo di esecuzione:%f \n", strCammino, profondita, tempo);
        free(cammino);
    }

    free(strCammino);
    fclose(camminiFile);
}

// Funzione che calcola i cammini minimi tra a e b, sfruttando l'algoritmo BFS
void *camminiMinimi(void *args)
{

    clock_t tempoInizio = times(NULL);

    // Inizializzo i dati
    calcolatore *dati = (calcolatore *)args;
    attore *grafo = dati->grafo;
    int numeroAttori = dati->numeroAttori;
    int sorgente = dati->a;
    int destinazione = dati->b;
    int trovato = 0;

    // Controllo se sorgente è valida
    attore *checkSorgente = bsearch(&sorgente, grafo, numeroAttori, sizeof(attore), comparaCodici);
    if (checkSorgente == NULL)
    {
        clock_t tempoFine = times(NULL);
        double tempoEsecuzione = (double)(tempoFine - tempoInizio) / sysconf(_SC_CLK_TCK);
        scriviCammini(sorgente, destinazione, NULL, -1, tempoEsecuzione);
        free(dati);
        return NULL;
    }

    // Controllo se destinazione è valida
    attore *checkDestinazione = bsearch(&destinazione, grafo, numeroAttori, sizeof(attore), comparaCodici);
    if (checkDestinazione == NULL)
    {
        clock_t tempoFine = times(NULL);
        double tempoEsecuzione = (double)(tempoFine - tempoInizio) / sysconf(_SC_CLK_TCK);
        scriviCammini(sorgente, destinazione, NULL, -2, tempoEsecuzione);
        free(dati);
        return NULL;
    }

    // Inizializzo la coda per l'algoritmo BFS
    int capacita = 1024; // dimensione iniziale della coda
    int inizio = 0;      // indice di inizio della coda
    int fine = 0;        // indice di fine della coda
    nodoAbr **coda = malloc(capacita * sizeof(nodoAbr *));
    if (!coda)
        xtermina("Errore: malloc coda", QUI);

    nodoAbr *alberoCopertura = NULL;
    nodoAbr *nodoSorgente = nuovoNodo(sorgente);

    enqueue(&coda, &fine, &capacita, &inizio, nodoSorgente); // inserisco la sorgente nella coda
    inserisci(&alberoCopertura, nodoSorgente, 0, NULL);      // inserisco la sorgente nell'albero di copertura

    while (!vuota(inizio, fine))
    {
        nodoAbr *nodoCorrente = dequeue(&coda, &inizio, &fine, &capacita); // estraggo il nodo corrente dalla coda
        if (nodoCorrente == NULL)
            xtermina("Errore: nodo corrente non trovato nell'albero di copertura", QUI);

        int codiceAttore = unshuffle(nodoCorrente->val);

        // prendo l'attore corrente dall'array di attori
        attore *attoreCorrente = bsearch(&codiceAttore, grafo, numeroAttori, sizeof(attore), comparaCodici);
        if (attoreCorrente == NULL)
            xtermina("Errore: stai cercando un attore inesistente", QUI);

        // Copio i dati dell'attore corrente nel nodo corrente
        nodoCorrente->nome = attoreCorrente->nome;
        nodoCorrente->anno = attoreCorrente->anno;

        // Controllo se ho trovato la destinazione
        if (codiceAttore == destinazione)
        {
            trovato = 1;
            break;
        }

        // Se non ho trovato la destinazione, continuo a cercare, inserendo i vicini nella coda, e continuando l'algoritmo BFS
        int *vicini = attoreCorrente->cop;
        int dimVicini = attoreCorrente->numcop;

        for (int i = 0; i < dimVicini; i++)
        {
            int codiceVicino = vicini[i];
            int codiceVicinoshuffle = shuffle(codiceVicino);

            // Controllo se il vicino è già stato visitato, se non lo è lo inserisco nella coda
            if (cerca(alberoCopertura, codiceVicinoshuffle) == NULL)
            {
                nodoAbr *nodoVicino = nuovoNodo(codiceVicino);
                enqueue(&coda, &fine, &capacita, &inizio, nodoVicino);
                inserisci(&alberoCopertura, nodoVicino, nodoCorrente->profondita + 1, nodoCorrente);
            }
        }
    }

    // termino l'algoritmo BFS e delloco ttte le risorse dopo aver scritto i cammini
    clock_t tempoFine = times(NULL);
    double tempoEsecuzione = (double)(tempoFine - tempoInizio) / sysconf(_SC_CLK_TCK);
    scriviCammini(sorgente, destinazione, alberoCopertura, trovato, tempoEsecuzione);
    free(dati);
    liberaAlbero(alberoCopertura);
    free(coda);
    return NULL;
}

// Funzione che crea la pipe, chiama il thread che calcola il cammino minimo passandogli i dati
void creaPipe(attore *attori, int numeroAttori, datiGestoreSegnali *dati)
{
    // Creo la pipe cammini.pipe
    if (mkfifo("cammini.pipe", 0666) != 0)
        xtermina("Errore: verificato nella creazione della named_pipe", QUI);

    // apro la pipe in lettura
    int fd = open("cammini.pipe", O_RDONLY);
    if (fd == -1)
        xtermina("Errore: verificato nell'apertura della pipe", QUI);

    xpthread_mutex_lock(dati->mutex, QUI);
    dati->faseLettura = 1; // Notifico al gestore segnali che la fase di lettura è iniziata
    xpthread_mutex_unlock(dati->mutex, QUI);

    while (1)
    {
        // Controllo se è stato richiesto di terminare (da gestore segnali)
        xpthread_mutex_lock(dati->mutex, QUI);
        int terminazione = dati->termina;
        xpthread_mutex_unlock(dati->mutex, QUI);
        if (terminazione) // se si termino
            break;

        coppia coppiaLetta;
        ssize_t e = read(fd, &coppiaLetta, sizeof(coppia));
        if (e == -1)
            xtermina("Errore nella lettura", QUI);
        if (e == 0)
            break;

        calcolatore *tcb = malloc(1 * sizeof(calcolatore)); // la struct che rappresenta il thread calcolatore
        // inizializzo i campi della struct
        tcb->a = coppiaLetta.a;
        tcb->b = coppiaLetta.b;
        tcb->grafo = attori;
        tcb->numeroAttori = numeroAttori;

        // creo il thread e lo rendo detached
        pthread_t threadCalcolo;
        xpthread_create(&threadCalcolo, NULL, camminiMinimi, tcb, QUI);

        int err = pthread_detach(threadCalcolo);
        if (err != 0)
            xtermina("Errore nell'utilizzo della detach", QUI);
    }

    sleep(20); // aspetto 20 secondi prima di terminare
    xclose(fd, QUI);
    unlink("cammini.pipe");
}

// Funzione che viene eseguita dal thread gestore segnali, dove appunto gestisce i segnali SIGINT e SIGUSR1
void *gestoreSegnali(void *args)
{
    datiGestoreSegnali *dati = (datiGestoreSegnali *)args;
    sigset_t mask;
    sigemptyset(&mask);        // creo l'insieme di segnali vuoto
    sigaddset(&mask, SIGINT);  // aggiungo il segnale SIGINIT all insieme di segnali bloccati
    sigaddset(&mask, SIGUSR1); // aggiungo il segnale SIGUSR1 all insieme di segnali bloccati
    printf("Il PID del processo è: %d\n", getpid());

    while (true)
    {
        int segnale;
        int e = sigwait(&mask, &segnale);
        if (e != 0)
            xtermina("Errore nella sigwait", QUI);

        if (segnale == SIGUSR1) // se il segnale è SIGUSR1, vuol dire che il main ha terminato quindi termino
            break;

        xpthread_mutex_lock(dati->mutex, QUI);
        if (dati->faseLettura == 0) // se non sono in fase di lettura lo notifico su stdout
        {
            fprintf(stderr, " Costruzione del grafo in corso\n");
            xpthread_mutex_unlock(dati->mutex, QUI);
        }
        else
        { // altrimenti segnala la terminazione al main e termino anche il gestore segnale
            dati->termina = 1;
            xpthread_mutex_unlock(dati->mutex, QUI);
            break;
        }
    }

    return NULL;
}

int main(int argc, char const *argv[])
{
    sigset_t mask;
    sigemptyset(&mask);                               // creo l'insieme di segnali vuoto
    sigaddset(&mask, SIGINT);                         // isnerisco SIGINT nell'insieme di segnali bloccati
    sigaddset(&mask, SIGUSR1);                        // isnerisco SIGUSR1 nell'insieme di segnali bloccati
    if (pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0) // blocco SIGINIT e SIGUSR1
        xtermina("Errore nel blocco del segnale SIGINT", QUI);

    // gestore segnali
    pthread_t trheadGestore;
    pthread_mutex_t mutextSegnali = PTHREAD_MUTEX_INITIALIZER; // mutex per sincronizzare l'accesso al file
    datiGestoreSegnali dati;
    dati.faseLettura = 0;
    dati.termina = 0;
    dati.mutex = &mutextSegnali;
    xpthread_create(&trheadGestore, NULL, gestoreSegnali, &dati, QUI); // creo il thread gestore segnali

    // Controllo degli argomenti passati
    if (argc != 4)
        xtermina("Il numero di argomenti passatto è errato", QUI);

    if (strcmp(argv[1], "nomi.txt") != 0 || strcmp(argv[2], "grafo.txt") != 0)
        xtermina("I file devono essere nomi.txt e grafo.txt", QUI);

    int numConsumatori = atoi(argv[3]);
    if (numConsumatori <= 0)
        xtermina("Il numero di consumatori deve essere maggiore di 0", QUI);

    int numeroAttori = contaAttori(argv[1]); // calcolo il numero di attori in testo.txt

    // array che contiene gli attori
    attore *attori = malloc(numeroAttori * sizeof(attore));
    if (attori == NULL)
        xtermina("C'è stato un errore nella creazione dell'array attori", QUI);

    inizializzaAttori(argv[1], attori); // inizializzo i campi codice,nome,anno di ogni attore

    produttore(argv[2], numConsumatori, numeroAttori, attori);

    creaPipe(attori, numeroAttori, &dati);

    // Terminazione tread gestoreSegnali e deallocazione finale,
    if (dati.termina == 0)                    // cioè se non è stato terminato il thread gestore
        pthread_kill(trheadGestore, SIGUSR1); // invio il segnale SIGUSR1 al thread gestore segnali, per farlo terminare

    xpthread_join(trheadGestore, NULL, QUI);     // aspetto che il thread gestore segnali termini
    xpthread_mutex_destroy(&mutextSegnali, QUI); // distruggo la mutex
    for (int i = 0; i < numeroAttori; i++)
    {
        free(attori[i].nome);
        free(attori[i].cop);
    }

    free(attori); // devo deallocare attori
}
