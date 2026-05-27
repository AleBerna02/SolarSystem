# Tappa 6
Il progetto è stato ideato da **Bernardello Alessio (S5592038)**, studente dell'università di Genova.

L'obbiettivo di quetsa tappa è di aggiungere la Luna che orbita attorno alla Terra, come fa quest'ultima con il Sole, andando ad aggiungere un nuovo nodo figlio al grafo di scena.

In questa tappa ho aggiunto/modificato le seguenti cose:
1. **Implementazione della Luna:** ho generato una terza mesh sferica molto piccola (ho impostato ```scale``` a 0.08f) ed impostandole un colore grigio roccia (```0.6f, 0.6f, 0.6f```), programmata in modo che ruoti attorno alla Terra;
2. **Separazione delle matrici:** ho diviso la matrice di trasformazione della Terra in due:
    * ```earthOrbit```: calcola e conserva soltanto la posizione spaziale della Terra nel sistema solare (rotazione dell'orbita e traslazione);
    * ```earthDraw```: prende ```earthOrbit``` e ci aggiunge la rotazione sul proprio asse ed il rimpicciolimento.


## Problemi riscontrati e soluzione
1. **La Luna uguale alla Terra:** inizialmente, quando ho creato la sfera della Luna, ho copiato esattamente la matrice della Terra per usarla come base per la Luna, questo però ha fatto si che la Luna diventasse minuscola perchè ha ereditato la scala di rimpicciolimento della Terra
    * dividendo la matrice di trasformazione ho iniziato la costruzione della Luna dalla matrice pulita ```earthOrbit```. Partendo da questo collegamento ho traslato successivamente la Luna della sua specifica distanza orbitale scalata (ovvero ```0.8f```). Ho applicato in seguito la sua rotazione e scala indipendente.
2. **Distorsione ottica massimizzando la finestra:** Nonostante nella tappa 02 avessi implementato il ```glViewport``` per mappare i pixel sulla nuova dimensione della finestra, massimizzando lo schermo la sfera appariva comunque schiacciata ed ovale.
    * Per risolvere prima di inviare la matrice ```proj``` alla GPU, ora viene calcolato il rapporto tra base e altezza in tempo reale (```float aspectRatio = (float)winWidth / (float)winHeight```). Così facendo qualsiasi siano le grandezze dello schermo i pianeti mantengono la loro grandezza.
# Comandi

Rispetto alle tappe precedenti non ho modificato i comandi, li riporto comunque qua sotto qual'ora dovessero servire:
* **Mouse e tasto sinistra:** permette una rotazione della telecamera;
* **Tasto L:** permette di attivare la visualizzazione Wireframe  mostrando quindi solo le linee della sfera;
* **Tasto F:** permette di attivare la visualizzazione solid, mostrando quindi solo le facce piene della sfera.

Riporto inoltre qua sotto un video del pianeta Terra, della Luna che orbitano una attorno all'altra ed entrambe che orbitano attorno al Sole sia in modalità Wireframe che in modalità solid:

![Animazione Sistema Solare](tappa6.gif)