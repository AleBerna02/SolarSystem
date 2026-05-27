# Tappa 2: Shading, wireframe e gestione del viewport
Il progetto è stato ideato da **Bernardello Alessio (S5592038)**, studente dell'università di Genova.

In questa seconda tappa il mio obbiettivo principale era di introdurre un riempimento della sfera in mdooc eh questa non mostri solo mesh triangolari ma una superficie con alcuni effetti di luce.

Rispetto alla tappa 1 ho aggiunto i seguenti punti:
1. **Shading procedurale:** ho modificato i due shader per dare una illusione di tridimensionalità senza però calcolare le luci reali:
    * **Vertex shader:** ho aggiunto la variabile ```out vec3 vPos``` per trasmetter le coordiante spaziali non trasformate al fragment shader;
    * **Fragment shader:** adesso calcola la distanza di un singolo fragmento dal centro dell'oggetto sugli assi X e Y (```length(vPos.xy)```). Andando a moltiplicare il colore base per un fattore inversamente proporzionale a questa distanza, sono riuscito a creare un colore più scuro sui bordi e più chiaro al centro per dare un senso di ombra.
2. **Aumento del dettaglio geometrico:** notando che nella tappa 1 la sfera sembrava avere poca curvatura, ho aumentato la risoluzione. Passando 40 anelli e 40 settori, ora la sfera ha una curvatura più morbida e una mesh più fitta.
3. **Rasterizzazione a Runtime:** per vedere la differenza dalla tappa 1 ho aggiunto un'ulteriore gestione degli input. Da tastiera controllo se viene premuto il pulsante "F" oppure "L", nel primo caso la sfera viene riempita attivnado la modalità ```GL_FILL```, mentre nel secondo caso la sfera viene svuotata mostrando soltanto le mesh ed attivando la modalità ```GL_LINE```.


## Problemi riconstrati e soluzione

1. **Deformazione dell'Aspect Ratio:** quando andavo a cliccare schermo intero nella barra della finestra la sfera si deformava, diventando ovale.
    * Il problema era dovuto al fatto che la matrice di proiezione manteneva il rapporto fisso 800x600 ignorando le nuove dimensioni. Per risolvere questo problema ho aggiunto due variabili dinamiche ```winWidth``` e ```winHeight```, ho adattato l'area del disegno  chiamando ```glViewport``` ed infine ho fatto in modo che il ricalcolo dell'Aspect ratio della matrice di proiezione prospettica avvenga in tempo reale (```glm::perspective((float)winWidth / (float)winHeight)```)

2. **Assenza di percezione della profondità:** usando un colore uniforme la sfera appariva solo un semplice cerchio 2D
    * Per risolvere questo problema (come già spiegato in precedenza nel punto 1 della prima sezione) ho fatto in modo che venisse trasmesso il vettore ```vPos``` dal Vertex al Fragment Shader. Così facendo ho potuto manipolare il colore di ogni pixel in base alla loro posizione sulla sfera.

## Comandi

Rispetto alla tappa 1 ho aggiunto due nuove comandi, di seguito riporto tutit quelli presenti nella tappa 2:
* **Mouse e tasto sinistra:** permette una rotazione della telecamera;
* **Tasto L:** permette di attivare la visualizzazione Wireframe  mostrando quindi solo le linee della sfera;
* **Tasto F:** permette di attivare la visualizzazione solid, mostrando quindi solo le facce piene della sfera.

Infine riporto di seguito un video della sfera che passa dalla modalità wireframe alla modalità solida, e viceversa:

![Animazione Sistema Solare](tappa2.gif)