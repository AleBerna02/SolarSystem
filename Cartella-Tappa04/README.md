# Tappa 4
Il progetto è stato ideato da **Bernardello Alessio (S5592038)**, studente dell'università di Genova.

Nella quarta tappa il mio obbiettivo era quello di far ruotare entrambi i pianeti sul proprio asse verticale. Ciò mi avrebbe permesso in futuro di riprodurre semplicemente quanto scritto in questa tappa per tutti i pianeti che avrei sviluppato nelle tappe precedenti.

Rispetto alla tappa 3 ho aggiunto/modificato:
1. **Il tempo:** ho incluso la libreria ```<SFML/System/Clock.hpp>``` e creato un orologio globale (```sf::Clock clock```). Adesso, ad ogni ciclo del ```while```, vado a prendere il tempo trascorso in secondi (usando ```clock.getElapsedTime().asSeconds()```);
2. **Matrici di rotazione:** ho aggiunto la funzione ```glm::rotate``` e la ho applicata alla matrice ```model```. Ho fatto ciò perchè l'angolo di rotazione delle sfere non è un valore fisso, ma è moltiplicato per il tempo trascorso, per evitare che la potenza della scheda video impatti sul tempo di rotazione. In questo modo vi è la stessa rotazione su ogni computer;
3. **Velocità indipendente:** ho assegnato ad ogni oggetto una velocità di rotazione diversa, in modo da poterne verificarne il corretto funzionamento ed avere anche una base utilizzabile per i pianeti aggiunti in seguito (infatti successivamente ogni pianeta avrà una rotazione unica). In questa tappa, la sfera gialla ruota con un moltiplicatore di ```0.5f```, mentre quella rossa con un moltiplicatore di ```1.5f```, risultando quindi più rapida della gialla.

## Problemi riconstrati e soluzione

1. **Ordine delle trasformazioni:** applicando le trasformazioni in maniera casuale, la sfera ruotava attorno al centro dello schermo con un raggio molto largo invece di girare su se stessa.
    * Per risolvere il problema è bastato modificare l'ordine dei calcoli. Prima ho applicato la traslazione (```glm::translate```) per spostare l'oggetto nella sua posizione finale per subito dopo applicare la rotazione (```glm::rotate```). Così facendo il vertice viene prima ruotato sul proprio asse locale e poi spostato nello spazio.
2. **Animazione legata agi FPS:** se avessi incrementato l'angolo di rotazione di un valore fisso ad ogni frame l'animazione sarebbe risultata velocissima su un monitor a 144hz mentre più lenta su uno a 60hz.
    * Come già spiegato in precedenza, per risolvere questo problema ho utilizzato il tempo in secondi generato da ```sf::Clock``` come moltiplicatore dell'angolo, cosi facendo la rotazione non è legata al numero di FPS.
3. **Bug nella lettura degli input:** una volta aggiunte le feature ho controllato se tutti i nuovi elementi sviluppati/modificati funzionassero correttamente. In questo modo ho potuto notare che i pulsanti "L" ed "F" non venivano gestiti correttamente a meno che non si tenesse premuto.
    * Per risolvere questo problema ho inserito la gestione dei tasti all'interno del ciclo di polling degli eventi (```while (window.pollEvent())```). Cosi facendo ogni pressione sui tasti viene intercettata e processata immediatamente dalla coda degli eventi.

## Comandi

Rispetto alle tappe precedenti non ho modificato comandi. In caso dovesssero servire, li riporto comunque qua sotto: 
* **Mouse e tasto sinistra:** permette una rotazione della telecamera;
* **Tasto L:** permette di attivare la visualizzazione Wireframe  mostrando quindi solo le linee della sfera;
* **Tasto F:** permette di attivare la visualizzazione solid, mostrando quindi solo le facce piene della sfera.

Riporto infine qua sotto un video delle due sfere che ruotano, sia in modalità wireframe sia in modalità solid:

![Animazione Sistema Solare](tappa4.gif)