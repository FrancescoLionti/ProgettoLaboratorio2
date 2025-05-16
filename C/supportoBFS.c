#include "supportoBFS.h"
#include "xerrori.h"
#define QUI __LINE__, __FILE__

// Funzione per inserire un nodo in coda(FIFO)
// La coda è implementata come un array dinamico di puntatori a nodoAbr
// La coda ha una capacità iniziale e viene riallocata se necessario
void enqueue(nodoAbr ***pcoda, int *fine, int *capacita, int *inizio, nodoAbr *nodo)
{
    nodoAbr **coda = *pcoda;

    // Se la coda è piena, raddoppia la sua capacità
    if (*fine >= *capacita)
    {
        *capacita *= 2; // Raddoppio la capacità
        coda = realloc(coda, (*capacita) * sizeof(nodoAbr *));
        if (!coda)
            xtermina("Errore: realloc in enqueue", QUI);

        *pcoda = coda; // Aggiorno il puntatore della coda
    }

    coda[(*fine)++] = nodo; // Aggiungo il nodo in coda e incrementa l'indice di fine

    // Dopo aver inserito un nodo, controllo la necessità di compattare la coda
    compatta(inizio, fine, pcoda);
}

// Funzione per estrarre un nodo dalla coda (FIFO)
// La coda è implementata come un array dinamico di puntatori a nodoAbr
// La coda ha una capacità iniziale e viene riallocata se necessario
nodoAbr *dequeue(nodoAbr ***pcoda, int *inizio, int *fine, int *capacita)
{
    nodoAbr **coda = *pcoda;
    if (*inizio < *fine) // Verifica se ci sono elementi da rimuovere
    {
        nodoAbr *res = coda[(*inizio)++]; // Estra il nodo in testa

        // Dopo aver rimosso un elemento, riduco la capacità se la coda è poco utilizzata
        riduciCapacita(pcoda, capacita, *fine);

        return res; // Restituisco il nodo estratto
    }
    return NULL; // La coda è vuota
}

// Funzione per compattare la coda, cioè spostare gli elementi a sinistra
void compatta(int *inizio, int *fine, nodoAbr ***pcoda)
{
    if (*inizio > 1000) // Se l'indice di inizio ha superato 1000
    {
        if (*fine - *inizio > 0) // Verifica che ci siano elementi da spostare
        {
            memmove(*pcoda, *pcoda + *inizio, (*fine - *inizio) * sizeof(nodoAbr *)); // Sposta gli elementi
            *fine -= *inizio;                                                         // Aggiorna l'indice di fine
            *inizio = 0;                                                              // Reset dell'indice di inizio
        }
    }
}

// Funzione per ridurre la capacità della coda
void riduciCapacita(nodoAbr ***pcoda, int *capacita, int fine)
{
    if (fine < *capacita / 2) // Verifica se la coda è sotto-utilizzata
    {
        *capacita /= 2;                                            // Dimezza la capacità
        *pcoda = realloc(*pcoda, (*capacita) * sizeof(nodoAbr *)); // Rialloca la memoria
        if (!*pcoda)
        {
            perror("Errore: realloc in riduciCapacita");
            exit(EXIT_FAILURE);
        }
    }
}

// Ritorna 1 quando inizio==fine cioè se la coda è vuota, altrimenti 0
int vuota(int inizio, int fine)
{
    return inizio == fine;
}

// Cerca il nodo con valore val nell'albero binario di ricerca
// se trovato: ritorna il puntatore al nodo, altrimenti NULL
nodoAbr *cerca(nodoAbr *radice, int val)
{
    if (radice == NULL)
        return NULL;

    if (val == radice->val)
        return radice;
    else if (val < radice->val)
        return cerca(radice->sx, val);
    else
        return cerca(radice->dx, val);
}

// Libera la memoria occupata dall'albero binario di ricerca
void liberaAlbero(nodoAbr *radice)
{
    if (radice == NULL)
        return;
    liberaAlbero(radice->sx);
    liberaAlbero(radice->dx);
    free(radice);
}

// Crea un nuovo nodo
nodoAbr *nuovoNodo(int val)
{
    nodoAbr *nodo = (nodoAbr *)malloc(sizeof(nodoAbr));
    nodo->val = shuffle(val);
    nodo->nome = NULL;    // Inizializza il nome a NULL
    nodo->anno = 0;       // Inizializza l'anno a 0
    nodo->sx = NULL;      // Inizializza il figlio sinistro a NULL
    nodo->dx = NULL;      // Inizializza il figlio destro a NULL
    nodo->padre = NULL;   // Inizializza il padre a NULL
    nodo->profondita = 0; // Inizializza la profondità a 0
    return nodo;
}

// Funzione che inserisce un nodo nell'albero binario di ricerca
void inserisci(nodoAbr **radice, nodoAbr *nodo, int profondita, nodoAbr *padre)
{

    if (*radice == NULL)
    {
        *radice = nodo;
        (*radice)->profondita = profondita; // Imposta la profondità del nodo
        (*radice)->padre = padre;           // Imposta il padre del nodo
        return;
    }

    if (nodo->val < (*radice)->val)
        inserisci(&((*radice)->sx), nodo, profondita, padre);
    else if (nodo->val > (*radice)->val)
        inserisci(&((*radice)->dx), nodo, profondita, padre);
}

// Funzione shuffle per mescolare il valore
int shuffle(int n)
{
    return ((((n & 0x3F) << 26) | ((n >> 6) & 0x3FFFFFF)) ^ 0x55555555);
}

// Funzione unshuffle per ripristinare il valore originale
int unshuffle(int n)
{
    return ((((n >> 26) & 0x3F) | ((n & 0x3FFFFFF) << 6)) ^ 0x55555555);
}

// Funzione per ricostruire il cammino dall'albero di copertura
// a partire dal nodo di destinazione fino alla sorgente
// La funzione prende in input la radice dell'albero, i valori a e b (sorgente e destinazione)
// e un array di puntatori a nodoAbr per memorizzare il cammino
void ricostruisciCammino(nodoAbr *radice, int a, int b, nodoAbr **cammino)
{
    nodoAbr *nodo = cerca(radice, shuffle(b)); // Trova il nodo della destinazione
    if (nodo == NULL)
        xtermina("Errore: Nodo non trovato nell'albero", QUI);

    int i = 0;
    while (nodo != NULL)
    {
        cammino[i++] = nodo; // Salvo il nodo nell'array
        nodo = nodo->padre;  // riasalgo al padre
    }

    // Verifica che cammino non sia vuoto
    if (i == 0)
        xtermina("Errore: Cammino vuoto", QUI);

    // Inverto il cammino, in modo che vada dalla srgente alla destinazione
    for (int j = 0; j < i / 2; j++)
    {
        nodoAbr *tmp = cammino[j];
        cammino[j] = cammino[i - j - 1];
        cammino[i - j - 1] = tmp;
    }
}
