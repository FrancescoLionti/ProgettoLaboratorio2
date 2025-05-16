# Progetto Completo Lab2

## Parsing delle righe del file di testo name.basics.tsv nel programma Java:
Il file name.basics.tsv contiene informazioni su persone del mondo cinematografico (attori, attrici, registi, ecc.).
L'obiettivo del parsing è selezionare solo attori e attrici con anno di nascita conosciuto e memorizzarli in una mappa, per un successivo utilizzo nella costruzione del grafo.
Ogni riga del file è composta da campi separati da tabulazioni (\t). Di conseguenza per effettuare il parsing delle righe del file name.basics.tsv, effettuo questa sequenza istruzioni:

1)**Ignorare l’intestazione** :La prima riga del file contiene le intestazioni dei campi, quindi viene saltata.

2)**Suddivisione della riga in campi**: Ogni riga successiva viene letta e suddivisa in un array di stringhe utilizzando:  
```String[] campi = linea.split("\t");``` cioè i campi vengono divisi in corrispondenza del carattere di tabulazione \t.

3)**Filtrare per anno di nascita:** Se il campo dell’anno di nascita contiene il valore \N (dato mancante), la riga viene ignorata
```if (campi[2].equals("\\N")) continue;```

4)**Controllo della profesione:** Controllo se tra le professioni elencate nella colonna primaryProfession è presente "actor" o "actress", tramite il metodo ```verificaAttore()``` che: 

- _Prende come Input:_ una stringa di lavori che rappresenta le professioni di una persona, separate da virgole.  
Ad esempio: "actor,director,producer" oppure "producer,actress".

- _Poi effetua lo Split delle professioni:_  La stringa viene suddivisa in un array chiamato professioni usando il metodo ```split(",")```.
Questo permette di ottenere ogni singola professione come elemento separato.

- _Ciclo su ogni professione_: Il ciclo for esamina ogni professione nell’array.
Per ogni professione, si eliminano eventuali spazi bianchi iniziali e finali ```(trim())``` e si converte tutta la stringa in minuscolo ```(toLowerCase())```.Questo serve per evitare errori dovuti a differenze di maiuscole/minuscole o spazi indesiderati (anche se non ci dovrebbero essere nel file.tsv), Poi viene fatto il controllo sul campo "pulito" 
- se la professione è esattamente "actor" oppure "actress", il metodo restituisce true, indicando che la persona è un attore o un’attrice.
- Se nessuna professione corrisponde: Dopo aver esaminato tutte le professioni, se nessuna è "actor" o "actress", il metodo restituisce false.

5)**Creazione dell’oggetto Attore e inserimento nella mappa:**
Se la riga supera tutti i controlli, viene creato un oggetto Attore con:  
- campi[0]: codice univoco (nconst), a cui poi verrà tolto "nm" e trasformato in un intero nel costruttore della classe Attore  
- campi[1]: nome dell’attore  
- campi[2]: anno di nascita, (viene trasformato in un intero nel costruttore della classe Attore)
L’attore viene infine aggiunto alla mappa attori, utilizzando come chiave il codice numerico derivato da nconst (es. "nm1234567" → 1234567):

## Implementazione della coda FIFO nell'algoritmo BFS, ed informazioni memorizzate in ogni elemento della coda:

### Iplementazione coda FIFO

La coda è implementata come un array dinamico di puntatori a nodi ```(nodoAbr **coda)```, con una capacità iniziale definita (```capacita = 1024```),.

Due indici gestiscono la coda:
- **inizio:** posizione del primo elemento nella coda (da cui si estrae).
- **fine**: posizione successiva all’ultimo elemento inserito (dove si inserisce).

Le operazioni principali sono:
### enqueue:
**Parametri**: puntatore alla coda, indici ```inizio``` e ```fine```, ```capacità corrente```, ```nodo da inserire```.  
**Scopo:** Inserisce un nuovo nodo in coda, alla posizione di inserimento (fine), aggiorna la capacità e compatta la coda se necessario.  
**Funzionamento**:
- Controlla se la coda è piena confrontando l’indice fine con la capacità attuale. 
- Se piena, raddoppia la capacità della coda riallocando memoria con ```realloc```.
- Inserisce il nuovo nodo nell’array in posizione ```fine``` e incrementa ```fine```.
- Chiama la funzione ```compatta``` per eventualmente spostare gli elementi a sinistra se l’indice inizio è diventato troppo grande, evitando sprechi di spazio.

### dequeue:
**Parametri:** puntatore alla coda, indici ```inizio``` e ```fine```, ```capacità corrente```.
**Scopo:** Estre (rimuove e restituisce) il nodo in testa alla coda (indice ```inizio```).
**Funzionamento:**
- Verifica che la coda non sia vuota (```inizio < fine```).
- Restituisce il nodo in posizione ```inizio``` e incrementa ```inizio```.
- Chiama la funzione ```riduciCapacita``` per ridurre la dimensione della coda se è sotto-utilizzata (per risparmiare memoria).
- Restituisce: il nodo estratto oppure ```NULL``` se la coda è vuota.

### compatta:
**Parametri**: indici ```inizio```, ```fine``` e ```puntatore alla coda```.  
**Scopo**: Compatta la coda spostando a sinistra gli elementi,evitando che l’indice inizio cresca indefinitamente. liberando così lo spazio all’inizio dell’array.  
**Funzionamento:**
- Se l’indice inizio supera 1000 (ovvero la coda ha consumato molta parte dello spazio iniziale), sposta tutti gli elementi da ```inizio``` a ```fine``` all’inizio dell’array.
- Aggiorna gli indici ```inizio``` e ```fine``` per riflettere la nuova posizione.

### riduciCapacita:
**Parametri**: puntaore alla coda, capacità attuale, indice ```fine```.  
**Scopo:** Riduce la capacità della coda se la memoria allocata è eccessiva rispetto agli elementi presenti.  
**Funzionamento:**  
- Se il numero di elementi (```fine```) è meno della metà della c```apacità allocata```, dimezza la ```capacità```.
- Rialloca la memoria con ```realloc``` per adattarla alla nuova dimensione.


### Informazioni memorizzate in ogni elemento della coda:
Ogni elemento della coda è un puntatore a un nodo di tipo nodoAbr, che contiene:
- **val**: codice identificativo dell’attore (nodo) rappresentato.
- **nome**: nome dell’attore, recuperato durante la visita.
- **anno**: anno di riferimento associato all’attore.
- **profondita**: distanza del nodo dalla sorgente (numero di "passi" o "archi").
- **padre**: puntatore al nodo genitore nell’albero di copertura, utile per ricostruire il cammino minimo.                          

Queste informazioni permettono di:
- Tenere traccia dello stato della visita.
- Ricostruire il percorso più breve una volta raggiunta la destinazione.
- Gestire efficientemente la visita dei nodi senza duplicati.

## Come vengono ricostruiti i nodi intermedi del cammino minimo in cammini.c :


## Come il thread gestore di segnali comunica al programma principale di interrompere l'elaborazione:


## Modo con cui vengono memorizzate e gestite le partecipazioni degli attori all'interno del programma Java:
