import java.util.Set;
import java.util.TreeSet;

public class Attore {
    /*
     * Rendo le variabili private per impedirne l'accesso e la modifica diretta
     * dall'esterno della classe
     */
    private int codice;
    private String nome;
    private int anno;
    private Set<Integer> coprotagonisti;
    private Set<Integer> films;

    public Attore(String codice, String nome, String anno) {

        /* Converto i campi ai rispettivi tipi */
        this.codice = Integer.parseInt(codice.substring(2)); // tolgo nm e poi converto in stringa
        this.nome = nome; // nome_cognome
        this.anno = Integer.parseInt(anno); // anno di nascita
        this.coprotagonisti = new TreeSet<Integer>();
        this.films = new TreeSet<Integer>();
    }

    /* I vari getter per ottenere i rispettivi valori */
    public int getCodice() {
        return codice;
    }

    public String getNome() {
        return nome;
    }

    public int getAnno() {
        return anno;
    }

    public Set<Integer> getCoprotagonisti() {
        return coprotagonisti;
    }

    public Set<Integer> getFilm() {
        return films;
    }

}
