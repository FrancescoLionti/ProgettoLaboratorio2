#!/usr/bin/env python3
import sys

def caricaPartecipazioniAttori(partecipazionitxt):
    """Leggo il file partec.txt prendo tutte le righe,per ogni riga prendo l'idAttore e i film a cui ha partecipato,
    e li metto in un dizionario, dove la chiave è l'idAttore e il valore è un set di film"""
    partecipazioniAttori ={} #contiene idAttore e tutti i film a cui ha partecipato
    with open(partecipazionitxt, 'r') as partecTxt:
        for line in partecTxt:
            campi=line.split('\t')
            film_set = set() #creo il set che contiene i codici film
            for film in campi[2:]:  #per ogni codice film
                film_codice = film.strip()  #tolgo eventuali spazi all'inizio e fine
                film_set.add(int(film_codice))  #poi inserisco il codice del titolo (castato come intero) nel set
            partecipazioniAttori[campi[0]] = film_set   #aggiungo il set come valore della chiave attore
    return partecipazioniAttori 

def verificaAttori(inputAttori):
    """Controllo se gli attori passati da riga di comando sono validi, se non lo sono
    stampo un messaggio di errore e chiudo il programma"""
    for attore in inputAttori:
        if attore not in partecipazioniAttori:
            print(f"Codice attore: {attore} non valido")
            sys.exit(0)
    
def caricaFilm(filmBasicTSV):
    """Leggo il file film.tsv prendo tutte le righe,per ogni riga prendo l'idFilm e il titolo del film,
    e li metto in un dizionario, dove la chiave è l'idFilm e il valore è il titolo del film"""
    films = {}
    with open(filmBasicTSV, 'r') as filmTSV:
        next(filmTSV) #salto la prima riga, che contiene i nomi delle colonne
        for line in filmTSV:
            campi=line.split('\t')
            codiceTitolo=campi[0]
            codiceTitolo=codiceTitolo[2:] #prendo il codice del film senza il prefisso "tt"
            films[int(codiceTitolo)] = campi[3] #associo alla chiave "codiceTitolo" il titolo del film
    return films

def coppieCollaborazioni(inputAttori):
    """Per ogni coppia di attori, prendo gli insiemi dei loro film e faccio l'intersezione
    in modo da  trvare i film a cui hanno partecipato insieme, poi stampo il risultato"""
    collabCoppie= {}
    for i in range(1, len(inputAttori)):
        idPrimoAttore=inputAttori[i-1]
        idSecondoAttore=inputAttori[i]
        coppiaAttori= (idPrimoAttore,idSecondoAttore) #chiave del dizionario collabCoppie
        collabCoppie[coppiaAttori]= partecipazioniAttori[idPrimoAttore] & partecipazioniAttori[idSecondoAttore] #creo l'intersezione tra i film a cui hanno partecipato i due attori
        numCollab=len(collabCoppie[coppiaAttori])# numero di film a cui hanno partecipato insieme

        if numCollab==0: # se non hanno mai collabborato insieme
            print(f"\n{idPrimoAttore}.{idSecondoAttore}: nessuna collaborazione")
            continue
    
        print(f"\n{idPrimoAttore}.{idSecondoAttore}: {numCollab} collaborazioni:")#stampa a schermo del risultato
        for codice in collabCoppie[coppiaAttori]:
            print(f"\n {codice} {films[codice]}") #stampo il codice film e il nomeOriginale
    
    return collabCoppie

#-------Main------- 

inputAttori=sys.argv[3:]# prendo gli IDattori da riga di comando e li salvo in una lista

partecipazioniAttori =caricaPartecipazioniAttori(sys.argv[1]) #carico le partecipazioni degli attori in un dizionario

verificaAttori(inputAttori) 

films = caricaFilm(sys.argv[2]) #carico i film in un dizionario

collabCoppie=coppieCollaborazioni(inputAttori) #carico le collaborazioni in un dizionario, e le stampo a schermo

print("\n==FINE==")
