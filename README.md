# Progetto Completo Lab2

# Parsing delle righe del file di testo name.basics.tsv nel programma Java:
Il file name.basics.tsv contiene informazioni su persone del mondo cinematografico (attori, attrici, registi, ecc.).
L'obiettivo del parsing è selezionare solo attori e attrici con anno di nascita conosciuto e memorizzarli in una mappa, per un successivo utilizzo nella costruzione del grafo.
Ogni riga del file è composta da campi separati da tabulazioni (\t). Di conseguenza per effettuare il parsing delle righe del file name.basics.tsv, effettuo questa sequenza istruzioni:

1)**Ignorare l’intestazione** :La prima riga del file contiene le intestazioni dei campi, quindi viene saltata.

2)**Suddivisione della riga in campi**: Ogni riga successiva viene letta e suddivisa in un array di stringhe utilizzando:  
```String[] campi = linea.split("\t");``` cioè i campi vengono divisi in corrispondenza del carattere di tabulazione \t.

3)**Filtrare per anno di nascita:** Se il campo dell’anno di nascita contiene il valore \N (dato mancante), la riga viene ignorata
```if (campi[2].equals("\\N")) continue;```

4)**Controllo della profesione:** Controllo se tra le professioni elencate nella colonna primaryProfession è presente "actor" o "actress", tramite il metodo ```verificaAttore()``` che: 

- **_Prende come Input:_** una stringa di lavori che rappresenta le professioni di una persona, separate da virgole.  
Ad esempio: "actor,director,producer" oppure "producer,actress".

- **_Poi effetua lo Split delle professioni:_** La stringa viene suddivisa in un array chiamato professioni usando il metodo ```split(",")```.
Questo permette di ottenere ogni singola professione come elemento separato.

- **_Ciclo su ogni professione_:** Il ciclo for esamina ogni professione nell’array.
Per ogni professione, si eliminano eventuali spazi bianchi iniziali e finali ```(trim())``` e si converte tutta la stringa in minuscolo ```(toLowerCase())```.Questo serve per evitare errori dovuti a differenze di maiuscole/minuscole o spazi indesiderati (anche se non ci dovrebbero essere nel file.tsv), Poi viene fatto il controllo sul campo "pulito" 
- se la professione è esattamente ```"actor"``` oppure ```"actress"```, il metodo restituisce ```true```, indicando che la persona è un attore o un’attrice.
- Se nessuna professione corrisponde: Dopo aver esaminato tutte le professioni, se nessuna è ```"actor"``` o ```"actress"```, il metodo restituisce ```false```.

5)**Creazione dell’oggetto Attore e inserimento nella mappa:**
Se la riga supera tutti i controlli, viene creato un oggetto Attore con:  
- ```campi[0]```: codice univoco (nconst), a cui poi verrà tolto "nm" e trasformato in un intero nel costruttore della classe Attore  
- ```campi[1]```: nome dell’attore  
- ```campi[2]```: anno di nascita, (viene trasformato in un intero nel costruttore della classe Attore)
L’attore viene infine aggiunto alla mappa attori, utilizzando come chiave il codice numerico derivato da nconst (es. "nm1234567" → 1234567):

# Implementazione della coda FIFO nell'algoritmo BFS, ed informazioni memorizzate in ogni elemento della coda:

## Iplementazione coda FIFO

La coda è implementata come un array dinamico di puntatori a nodi ```(nodoAbr **coda)```, con una capacità iniziale definita (```capacita = 1024```),.

Due indici gestiscono la coda:
- ```inizio```: posizione del primo elemento nella coda (da cui si estrae).
- ```fine```: posizione successiva all’ultimo elemento inserito (dove si inserisce).

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


## Informazioni memorizzate in ogni elemento della coda:
Ogni elemento della coda è un puntatore a un nodo di tipo nodoAbr, che contiene:
- ```val```: codice identificativo dell’attore (nodo) rappresentato.
- ```nome```: nome dell’attore, recuperato durante la visita.
- ```anno```: anno di riferimento associato all’attore.
- ```profondita```: distanza del nodo dalla sorgente (numero di "passi" o "archi").
- ```padre```: puntatore al nodo genitore nell’albero di copertura, utile per ricostruire il cammino minimo.                          

Queste informazioni permettono di:
- Tenere traccia dello stato della visita.
- Ricostruire il percorso più breve una volta raggiunta la destinazione.
- Gestire efficientemente la visita dei nodi senza duplicati.

# Come vengono ricostruiti i nodi intermedi del cammino minimo in cammini.c :
Una volta che l’algoritmo BFS ha trovato la destinazione, il cammino minimo non viene restituito direttamente, ma viene ricostruito risalendo l’albero di copertura generato durante la visita, questo viene svolto dalla funzione ```ricostruisciCammino```

## ricostruisciCammino(nodoAbr *radice, int a, int b, nodoAbr **cammino)
si occupa di risalire dai nodi foglia (```destinazione```) fino alla radice (```sorgente```) dell’albero, sfruttando i puntatori al padre (```padre```) memorizzati in ogni nodo. In questo modo vengono raccolti, tutti i nodi che compongono il cammino minimo tra ```a``` e ```b```.  

#### Parametri
 - ```nodoAbr *radice```: È la radice dell’albero di copertura costruito durante la BFS.
 - ```int a```: Codice identificativo dell’attore sorgente (punto di partenza del cammino).
 - ```int b```: Codice identificativo dell’attore destinazione (punto di arrivo del cammino).
 - ```nodoAbr **cammino```: È un array di puntatori a ```nodoAbr```, passato per riferimento, in cui verrà salvato il cammino minimo da ```a``` a ```b```.  

### Funzionamento
- **Ricerca del nodo di destinazione:**  
Si parte cercando, all'interno dell’albero di copertura ```radice```, il nodo corrispondente alla destinazione ```b```.
La ricerca utilizza il valore ```shuffle(b)``` per trovare il nodo giusto (lo stesso valore usato nella BFS).  
- **Risalita tramite il campo padre:** Una volta trovato il nodo, si risale lungo i puntatori ```padre```, aggiungendo ciascun nodo in un array```cammino```.
Questo processo continua fino ad arrivare al nodo radice (che ha padre == NULL).  
- **Inversione dell’array:** Poiché i nodi sono stati salvati dalla ```destinazione``` alla ```sorgente```, effettuo un'inversione dell'array per ottenere il cammino nell’ordine corretto: ```sorgente``` → … → ```destinazione```.

## Come il thread gestore di segnali comunica al programma principale di interrompere l'elaborazione:
Il meccanismo di comunicazione tra il thread gestore di segnali e il programma principale si basa su una struttura dati condivisa ```(datiGestoreSegnali)``` protetta da un mutex:
```
typedef struct {
    int faseLettura;        // 0 = non iniziata, 1 = lettura pipe avviata
    int termina;            // 0 = continua, 1 = interrompi elaborazione
    pthread_mutex_t *mutex; // protezione accessi concorrenti
} datiGestoreSegnali;
```
Il procedimento è il seguente: 
### Inizializzazione:
Nel main,viene creata una struct``` dati``` di tipo ```datiGestoreSegnali```, poi  ```dati.faseLettura``` ed ```dati.termina``` sono inizializzati a 0, ed viene anche assegnata la muetex ad ```dati.mutex```. In seguito il main blocca i segnali ```SIGINT``` e ```SIGUSR1``` con ```pthread_sigmask```.
E come ultimo si crea il thread gestore segnali con ```xpthread_create(&trheadGestore, NULL, gestoreSegnali, &dati, QUI)```;

#### gestoreSegnali:
Il thread gestoreSegnali svolge il compito di intercettare in modo sincrono i segnali ```SIGINT``` (generato da CTRL+C) e ```SIGUSR1``` (utilizzato internamente per segnalare al gestore di terminare).
All’avvio, il gestore crea una maschera ```mask``` contenente ```SIGINT``` e ```SIGUSR1``` e si posiziona in attesa con ```sigwait```.

```SIGINT```: a seguito di una pressione di CTRL+C, il gestore riceve ```SIGINT``` e valuta lo stato attuale del programma tramite il campo ```faseLettura``` della struttura condivisa:

- Se la lettura dalla pipe non è ancora iniziata (```faseLettura == 0```), il gestore stampa un messaggio informativo su stderr, avvisando l’utente che la fase di costruzione del grafo è in corso
- Se la lettura è già in corso (```faseLettura == 1```), il gestore riconosce che il programma è nella fase critica di elaborazione e decide di interrompere l’operazione. Accedendo in mutua esclusione al campo ```termina```, imposta ```termina = 1```, segnalando così al thread di lettura (cioè il main) la richiesta di arrestarsi. Subito dopo, il gestore esce dal loop di ascolto e termina.

SIGUSR1: quando il main decide che l’applicazione ha completato le sue operazioni (ad esempio, al termine della lettura dalla pipe e dopo aver gestito tutti i calcoli), invia SIGUSR1 al gestore. Il thread riceve tale segnale e, riconoscendolo come istruzione di chiusura, esce immediatamente dal ciclo di ascolto e termina.
# Modo con cui vengono memorizzate e gestite le partecipazioni degli attori all'interno del programma Java:
## Strutture con cui memorizzo i dati necessari

Ogni attore è rappresentato da un’istanza della classe Attore, che contiene:
- ```int codice```: identificativo numerico dell’attore (estratto da nconst, rimuovendo "nm").
- ```String nome```: nome dell’attore.
-  ```int anno```: anno di nascita.
- ```Set<Integer>``` coprotagonisti: insieme ordinato (```TreeSet```) dei codici degli altri attori con cui ha recitato.
- ```Set<Integer> films```: insieme ordinato dei codici dei film a cui ha partecipato.  
Queste strutture dati sono private e accessibili solo tramite i metodi getter, garantendo l'incapsulamento, di cui ovviamente ho fornito la classe Attore.

#### Mappa Map<Integer, Attore> attori
Tutti gli attori vengono memorizzati in una TreeMap, dove:
- La chiave è il codice numerico dell’attore.
- Il valore è l’oggetto Attore associato.
- Questa struttura mantiene gli attori ordinati per codice.

#### Mappa Map<String, Set<Integer>> castMap
Durante la lettura del file ```title.principals.tsv```, costruisco una mappa ```castMap``` dove:
- La chiave è il _codice del film_ (tconst).
- Il valore è un ```TreeSet``` di codici attori (Integer) che compongono il cast del film, ovviamente ordinati 

## Gestione delle partecipazioni:
La gestione effettiva delle partecipazioni degli attori ai film avviene quando il cast di un film è completo. In quel momento, viene chiamata la funzione: ```aggiornaFilms(entry.getValue(), attori, entry.getKey());``` che svolge alcuni passi:

- **Conversione del codice film:** Viene rimosso il prefisso "tt" dalla stringa film e il resto viene convertito in int, per uniformarlo al formato usato nella classe ```Attore```.
- **Aggiornamento dei singoli attori:** Si scorre ogni componente del cast del film. Se l’attore è presente nella mappa ```attori``` cioè se è un attore valido, allora si aggiunge il ```codiceFilm``` al suo set di ```film```.



