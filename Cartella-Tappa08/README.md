# Tappa 8
Il progetto è stato ideato da **Bernardello Alessio (S5592038)**, studente dell'università di Genova.

Anche in questa fase mi sono posto due obbiettivi:
1. Aggiungere le texture ai vari pianeti;
2. Implementare la possibilità di premere un pulsante per fermare il tempo, in modo da poter visionare tutti i pianeti.

In questa tappa ho aggiunto/modificato le seguenti cose:
1. **Generazione delle coordiante per le texture:** Ho aggiunto dei parametri alla funzione`generateSphere`. Oltre alle coordinate `(X, Y, Z)`, ora calcola anche le coordinate di texture `(U, V)` per ogni vertice, mappando la griglia bidimensionale dell'immagine sulla superficie tridimensionale della sfera.
2. **Caricamento delle texture:** Ho creato la funzione `loadTexture` che sfrutta la libreria SFML per leggere i file `.jpg`, decodificarli in un array di pixel e inviarli alla memoria della GPU generando un ID texture (`glGenTextures`).
    * Il Fragment Shader non usa più la variabile `objectColor`, ma un `sampler2D` per campionare il colore del pixel direttamente dall'immagine caricata.
3. **Aggiornamento della struttura dati:** La `struct Planet` non memorizza più un vettore RGB per il colore, ma conserva l'`unsigned int textureID` associato all'immagine del pianeta.

## Problemi riscontrati e soluzione
1. **Le texture capovolte:** Una volta caricate le immagini, i poli dei pianeti risultavano invertiti e le mappe erano sottosopra.
    * Per risolvere il problema all'interno della funzione `loadTexture`, subito dopo aver caricato l'immagine in RAM con SFML, vado a chiamare la funzione `img.flipVertically()`. Questa inverte l'array di pixel prima di inviarlo alla scheda video, allineandolo perfettamente al sistema di coordinate di OpenGL.
2. **Mappare l'immagine 2D su una sfera:** Dovevo trovare un modo per far si che le texture (rettangolari) avvolgessero una sfera , perchè l'immagine si spalmava su tutta la sfera in modo casuale;
    * Per risolvere il problema all'interno dei due cicli `for` che generano la sfera matematica, ho aggiunto il calcolo delle coordinate UV proporzionalmente all'avanzamento dei cicli: `float u = (float)s / (sectors - 1)` e `float v = (float)r / (rings - 1)`. In questo modo la coordinata U (orizzontale) va gradualmente da 0.0 a 1.0 lungo i "meridiani", e la V (verticale) lungo i "paralleli".
3. **Il salto di tempo nella pausa:** Inizialmente l'animazione si basava sul tempo assoluto dall'avvio del programma (`clock.getElapsedTime()`).  Quando ho implementato la pausa mi sono trovato davanti al problema di "teletrasporto die pianeti", se inizialmente rimanevano fermi nella posizione in cui venivano bloccato, appena si ripremeva il pulsante per partire questi si "teletrasportavano" più avanti come se non si fosserro mai fermati.
    * Ho rimosso il tempo assoluto in modo da aggiunger **Delta Time**. Ora `float deltaTime = clock.restart().asSeconds()` misura i millisecondi passati dall'ultimo frame. Ho fatto in modo che questo valore venisse sommato a una nuova variabile `accumulatedTime`. Cosi facendo quando si preme pausa (`isPaused = true`), il programma smette di sommare il `deltaTime` all'accumulatore, fermando i pianeti esattamente dove si trova senza salti alla ripresa.

## Provenienza delle texture
Tutte le texture sono state scaricate dal seguente sito:
https://www.solarsystemscope.com/textures/ .

## Comandi
rispetto alla tappa precedente ho aggiunto il comando "P" per mettere in pausa tutti i pianeti ed il sole. riporto la lista completa dei comandi con aggiunta del nuovo:
* **Mouse e tasto sinistra:** permette una rotazione della telecamera;
* **Rotellina Mouse:** zoom in/zoom out;
* **Tasto L:** permette di attivare la visualizzazione Wireframe  mostrando quindi solo le linee della sfera;
* **Tasto F:** permette di attivare la visualizzazione solid, mostrando quindi solo le facce piene della sfera;
* **Tasto P:** permette di fermare la rotazione dei pianeti.

Di seguito riporto un video che mostra tutte le funzionalità aggiunte con questa tappa:

![Animazione Sistema Solare](tappa8.gif)