#ifndef SUPPORTOBFS_H
#define SUPPORTOBFS_H

// Struttura che rappresenta un nodo dell'albero binario di ricerca
typedef struct nodoAbr
{
    int val;               // codice attore
    char *nome;            // nome attore
    int anno;              // anno di nascita
    struct nodoAbr *sx;    // figlio sinistro
    struct nodoAbr *dx;    // figlio destro
    struct nodoAbr *padre; // padre del nodo, serve per ricostruire il cammino
    int profondita;        // profondità del nodo nell'albero di copertura
} nodoAbr;

// Funzioni per la gestione della coda

void enqueue(nodoAbr ***pcoda, int *fine, int *capacita, int *inizio, nodoAbr *nodo);
nodoAbr *dequeue(nodoAbr ***pcoda, int *inizio, int *fine, int *capacita);
int vuota(int inizio, int fine);
void compatta(int *inizio, int *fine, nodoAbr ***pcoda);
void riduciCapacita(nodoAbr ***pcoda, int *capacita, int fine);

// Funzioni per la gestione dell'albero binario di ricerca

nodoAbr *cerca(nodoAbr *radice, int val);
void stampaInOrdine(nodoAbr *radice);
void liberaAlbero(nodoAbr *radice);
nodoAbr *nuovoNodo(int val);
void inserisci(nodoAbr **radice, nodoAbr *nodo, int profondita, nodoAbr *padre);
int unshuffle(int n);
int shuffle(int n);
void ricostruisciCammino(nodoAbr *radice, int a, int b, nodoAbr **cammino);

#endif // SUPPORTOBFS_H