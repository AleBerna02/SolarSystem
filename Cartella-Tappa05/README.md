# Tappa 5
Il progetto è stato ideato da **Bernardello Alessio (S5592038)**, studente dell'università di Genova.

In questa tappa ho deciso di implementare la rotazione di una sfera attorno ad un'altra e non più solo su se stessa.

Rispetto alla tappa precedente ho aggiunto/modificato:

1. **Il sole:** ho spostato la prima sfera gialla implementata al centro della scena e le ho applicato una rotazione lenta e continua (```time * 0.2f```). Questa sfera andrà a rappresentare  la parte centrale del futuro sistema solare;
2. **La terra:** la seconda sfera adesso non è più fissa ma gli sono state applicate una catena di trasformazioni per: rimpicciolirla (```glm::scale``` a 0.03f), farla ruotare su se stessa più velocemente del sole (```time * 2.0f```), allontanarla dal centro (```glm::translate``` di 4.0f) ed infine farla orbitare attorno al sole (```time * 0.6f```);
3. **Riposizionamento della  telecamera:** per inquadrare l'orbita completa, ho arretrato la telecamera (impostando ```rho = 8.0f```) ed la ho inclinata leggermente verso l'alto impostando un angolo di partenza di ```theta = 0.5f``` In questo modo ho ottenuto una prospettiva dall'alto più chiara.

## Problemi riconstrati e soluzione
1. **Come far orbitare un oggetto:** il problema più complesso che ho dovuto affrontare è stato il trovare un modo per applicare le trasformazioni in modo che il secondo pianeta non sia soltanto distante e giri su se stesso ma che giri ANCHE intorno al sole.
    * Ho trovato la solizione nell'ordine di moltiplicazione delle matrici. Ricontrollando, infatti, le slide fornite dai professori, ho chiarito che queste vengono applicate da destra verso sinistra (quindi dal basso verso l'alto). Ho applicato: 
        * **In fondo:** Scale, per rimpicciolire la terra;
        * **Sopra:** Rotate, per far girare la terra sul proprio asse locale;
        * **Sopra ancora:** Translate, per far spostare la terra a 4.0 unità di distanza;
        * **In cima:** Rotate, per far ruotare tutto lo spazio traslato attorno all'origine creando così definitivamente la prima orbita.
2. **Gestione della scala e distanza:** Se avessi usato l'esatta scala di distanza dei pianeti, nessun PC avrebbe retto il carico richiesto.
    * Ho applicato quindi un modello divulgativo, andando ad impostare la terra ad una distanza arbitraria a ```4.0f``` unità dal centro del sole per dare un'idea della distanza reale.
3. **Sovrapposizione cromatica:** avendo entrambe le sfere di raggio 1.0, la terra sarebbe stata della stessa grandezza del sole e ciò non sarebbe stato corretto. 
    * Per risolvere ho introdotto la matrice di scalameno (```glm::scale```) passandole il vettore ```(0.3f, 0.3f, 0.3f)```. Cosi facendo il volume della terra viene ridotto del 30% rispetto a quello del sole. Un'altro cambiamento è stato il colore della terra, che ora risulterà blu grazie ad una modifica dei colori a (```0.2f, 0.4f, 0.9f```).

# Comandi 
Rispetto alle tappe precedenti non ho modificato comandi.  In caso dovesssero servire, li riporto comunque qua sotto: 
* **Mouse e tasto sinistra:** permette una rotazione della telecamera;
* **Tasto L:** permette di attivare la visualizzazione Wireframe  mostrando quindi solo le linee della sfera;
* **Tasto F:** permette di attivare la visualizzazione solid, mostrando quindi solo le facce piene della sfera.

Riporto, inoltre, qua sotto un video del pianeta terra che orbita intorno al sole, sia in modalità Wireframe che in modalità solid:

![Animazione Sistema Solare](tappa5.gif)