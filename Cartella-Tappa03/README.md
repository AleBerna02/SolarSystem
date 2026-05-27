# Tappa 3
Il progetto è stato ideato da **Bernardello Alessio (S5592038)**, studente dell'università di Genova.

In questa terza tappa il mio obbiettivo principale era di passare dal rendering di un singolo oggetto alla gestione di più oggetti in uno spazio 3D. Per fare ciò, ho posizionato due sfere diverse (una gialla ed una rossa) in modo da verificare come questo impatta a livello grafico.

Rispetto alla tappa 2 ho aggiunto/cambiato i seguenti punti:

1. **Colore fisso delle sfere:** nel codice della tappa 2 l'unico colore presente era il giallo. Aggiungendo una seconda sfera questo non andava però più bene perchè quest'ultima veniva anch'essa colorata di giallo. Ho quindi creato una nuova variabile (```uniform vec3 objectColor;```) che faccia sì che venga modificato automaticamente il colore del frammento;
2. **Traslazione spaziale:** ho utilizzato ```glm::translate``` per poter modificare la matrice ```model``` e spostare le sfere dall'origine. La sfera gialla l'ho spostato a sinistra di -1.5f, mentre la sfera rossa a destra di +1.5f;
3. **Aggiustamento dell'inquadratura:** una volta aver aggiunto la seconda sfera, mi sono reso conto che la distanza della telecamera non era sufficiente per mostrare entrambe le sfere. Ho quindi aumentato il valore della ```rho``` da ```4.0f``` a ```7.0f```, espandendo cosi il campo visivo.

## Problemi e soluzioni

1. **Ripetizione inutile del codice shader:** come detto precedentemente, nella tappa 2 il codice imponeva il colore giallo a qualsiasi entità venisse disegnata. Per disegnare il pianeta rosso avrei potuto creare un secondo Fragment Shader dedicato solo al rosso, tuttavia questo avrebbe allungato il mio codice rendendolo confusionario, sopratutto per le tappe successive.
    * Per risolvere il problema ho aggiunto una variabile ```uniform``` che ha reso lo shader universale. Inoltre, dalla tappa 3 in avanti, prima di chiamare la funzione di disegno, la CPU aggiorna dinamicamente la GPU sul colore da utilizzare tramite il comando ```glUniform3f(..., R, G, B)```.
2. **Saturazione della memoria video per oggetti simili:** disegnare più pianeti creando per ognuno un proprio Vertex BufferObject avrebbe comportato uno spreco di memoria VRAM, essendo che lo avrei dovuto utilizzare per il disegno di ogni sfera.
    * Per risolvere questo problema adesso la mesh sferica viene calcolata ed inviata alla GPU una singola volta. Per disegnare, invece, due sfere in punti diversi basta bindare il VAO, aggiornare la matrice di traslazione ed il colore ed, infine, richiamare due volte consecutive ```glDrawElements```.

## Comandi

Rispetto alla tappa 2 non ho modificato comandi, in quando tutti i già presenti comandi si applicano ad entrambe le sfere del mondo. Detti comandi sono:
* **Mouse e tasto sinistra:** permette una rotazione della telecamera;
* **Tasto L:** permette di attivare la visualizzazione Wireframe  mostrando quindi solo le linee della sfera;
* **Tasto F:** permette di attivare la visualizzazione solid, mostrando quindi solo le facce piene della sfera.

Riporto di seguito un video delle due sfere, entrambe passano dalla modalità wireframe alla modalità solida:

![Animazione Sistema Solare](tappa3.gif)