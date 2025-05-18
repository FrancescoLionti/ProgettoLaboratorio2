import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;

public class CreaGrafo {
    public static void main(String[] args) {

        Map<Integer, Attore> attori = new TreeMap<>(); // Mappa che conterrà gli attori ordinati per codice

        /* Lettura del file name.basics.tsv */
        try {
            BufferedReader br = new BufferedReader(new FileReader(args[0]));
            String linea = br.readLine();// salta la prima linea contenente le intestazioni "inutili"

            while ((linea = br.readLine()) != null) {

                String[] campi = linea.split("\t");/* salvo in un array i campi, separati dal tab */
                if (campi[2].equals("\\N")) /* salta le stringhe contententi \N come data di nascita */
                    continue;

                boolean isActor = verificaAttore(campi[4]);/* verifica se la persona è un attore */
                if (!isActor)
                    continue;

                /* Creo l'attore e lo inserisco con il suo rispettivo codice nella Map */
                Attore attore = new Attore(campi[0], campi[1], campi[2]);
                attori.put(attore.getCodice(), attore);
            }

            br.close();
        } catch (Exception e) {
            System.err.println(e + "\n Errore riscontrato nella lettura del file name.basics.tsv");
        }

        /* Scrittura del file testo.txt */
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter("nomi.txt"));

            /* Scrivo nel file i dati di tutti gli attori, nel fomato TSV */
            for (Attore attore : attori.values()) {
                StringBuilder linea= new StringBuilder();
                linea.append(attore.getCodice()).append("\t").append(attore.getNome()).append("\t").append(attore.getAnno());
                bw.write(linea.toString());// scrivo la linea in fomato TSV
                bw.newLine();// vado a capo
            }
            bw.close();

        } catch (Exception e) {
            System.err.println(e + "\nErrore riscontrato nella scrittura del file nomi.txt");
        }

        /* Lettura del file: title.principals.tsv */
        try {

            BufferedReader br = new BufferedReader(new FileReader(args[1]));
            String linea = br.readLine(); // intestazione
            if (linea == null) throw new IOException("File vuoto");
        
            String filmCorrente = null;
            Set<Integer> castCorrente = new TreeSet<>();
        
            while ((linea = br.readLine()) != null) {
                String[] campi = linea.split("\t");
                String codiceFilm = campi[0];
                Integer codiceAttore = parseCodiceAttore(campi[2]);
        
                if (!attori.containsKey(codiceAttore)) continue;
        
                if (!codiceFilm.equals(filmCorrente)) {
                    if (filmCorrente != null) {
                        // Aggiorna collaborazioni e film per il cast precedente
                        aggiornaCollaborazioni(castCorrente, attori);
                        aggiornaFilms(castCorrente, attori, filmCorrente);
                    }
                    filmCorrente = codiceFilm;
                    castCorrente = new TreeSet<>();
                }
                castCorrente.add(codiceAttore);
            }
            // Aggiorna per l'ultimo film letto
            if (filmCorrente != null) {
                aggiornaCollaborazioni(castCorrente, attori);
                aggiornaFilms(castCorrente, attori, filmCorrente);
            }
            br.close();

        } catch (Exception e) {
            System.err.println("\nErrore riscontrato nella lettura di title.principals.tsv" + e);
        }

        /*
         * Scrittura del file grafo.txt
         */
        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter("grafo.txt"));

            for (Attore attore : attori.values()) {
                StringBuilder linea = new StringBuilder();// StringBuilder per costruire la linea in modo efficiente
                linea.append(attore.getCodice());

                Set<Integer> coprotagonisti = attore.getCoprotagonisti(); // Set di coprotagonisti dell'attore
                if (coprotagonisti == null) {
                    bw.close();
                    throw new IllegalArgumentException("\n coprotagonisti null per l'attore: " + attore.getCodice());
                } else {
                    linea.append("\t").append(coprotagonisti.size());
                    for (Integer c : coprotagonisti) {
                        linea.append("\t").append(c);
                    }
                }

                bw.write(linea.toString());
                bw.newLine();
            }

            bw.close();

        } catch (Exception e) {
            System.err.println("\nErrore riscontrato durante la scrittura del file grafo.txt" + e);
        }

        try {
            BufferedWriter bw = new BufferedWriter(new FileWriter("partecipazioni.txt"));

            for (Attore attore : attori.values()) {
                StringBuilder linea = new StringBuilder();// StringBuilder per costruire la linea in modo efficiente
                linea.append(attore.getCodice()).append("\t").append(attore.getFilm().size());

                /* aggiungo alla riga i coprotagonisti dell'attore in ordine crescente */
                for (Integer film : attore.getFilm()) {
                    linea.append("\t").append(film);
                }

                bw.write(linea.toString());
                bw.newLine();
            }
            bw.close();

        } catch (Exception e) {
            System.err.println("\nErrore riscontrato durante  la scrittura del file partecipazioni.txt" + e);
        }
    }

    /* veriica se la persona è un attore o attrice */
    private static boolean verificaAttore(String lavori) {
        String[] professioni = lavori.split(",");
        for (String professione : professioni) {

            String p = professione.trim().toLowerCase(); // toglie spazi ad inizio e fine,trasforma tutto in minuscolo
            if (p.equals("actor") || p.equals("actress")) {
                return true;

            }
        }
        return false;
    }

    private static Integer parseCodiceAttore(String nconst) {
        // toglie nm alla stringa e poi lo converte in Integer
        return Integer.parseInt(nconst.substring(2));
    }

    private static void aggiornaCollaborazioni(Set<Integer> cast, Map<Integer, Attore> attori) {
        // Converto il set in una lista per poter accedere agli elementi tramite indici
        List<Integer> lista = new ArrayList<>(cast);
        // Per ogni coppia distinta di attori nel cast
        for (int i = 0; i < lista.size(); i++) {
            for (int j = i + 1; j < lista.size(); j++) {
                Integer att1 = lista.get(i);
                Integer att2 = lista.get(j);
                // Verifico che entrambi gli ID siano presenti nella mappa attori
                if (attori.containsKey(att1) && attori.containsKey(att2)) {
                    // Aggiungo att2 come coprotagonista di att1 e viceversa
                    attori.get(att1).getCoprotagonisti().add(att2);
                    attori.get(att2).getCoprotagonisti().add(att1);
                }
            }
        }

    }

    public static void aggiornaFilms(Set<Integer> cast, Map<Integer, Attore> attori, String film) {
        final int codiceFilm = Integer.parseInt(film.substring(2));

        // Aggiorno la lista dei film per ogni attore
        for (Integer idAttore : cast) {
            Attore attore = attori.get(idAttore);
            if (attore != null) {
                attore.getFilm().add(codiceFilm);
            }
        }
    }

}
