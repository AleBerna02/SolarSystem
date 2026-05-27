# Tappa 7
Il progetto è stato ideato da **Bernardello Alessio (S5592038)**, studente dell'università di Genova.

Per questa tappa mi sono posto due obbiettivi:
1. Aumentare il numero di pianeti presenti da 2 ad 8;
2. Sostituire il modello di luce con un modello di illuminazione 3D.

In questa tappa ho aggiunto/modificato le seguenti cose:
1. **Illuminazione diffusa:** ho riscritto il Vertex shader che ora calcola e manda al Fragment Shader la posizione del frammento (```FragPos```) e la sua normale matematica (```Normal```). Il Fragmen shader calcola ora l'angolo tra la normale e la fonte di luce fissa al centro (```lightPos```), illuminando solo la faccia esposta al sole;
2. **Architettura Data-Driven:** volevo ottimizzare il modo in cui venivano salvate le informazioni di ogni pianeta, per cui ho utilizzato una struttura dati chiamata ```Planet``` che raggurppa le caratteristiche univoche di ogniu pianeta (quindi disatnza, dimensione, velocità e colore). Ho inserito i pianeti in un vettore e rendirizzati dinamicamente tramite un ciclo ```for```.
3. **Zoom della telecamera:** ho aggiunto un controllo sull'evento dell'utilizzo della rotellina (```sf::Event::MouseWheelScrolled```) per modificare la distanza ```rho``` della telecamera (con i valori che devono stare tr ```2.0f``` e ```80.0f```), permettendo di zoomare o dezoomare.

## Problemi riscontrati e soluzione
1. **Ombreggiatura errata durante la rotazione:** Mentre i pianeti ruotavano la faccia del pianeta che guarda il sole deve rimanere illuminata, mentre la parte che non guarda il sole deve rimanere scura. Inizialmente, passando la normale pura del vertice al Fragment Shader, l'illuminazione si attaccava  alla mesh e ruotava insieme ad essa, dando il risultato opposto a quello che cercavo.
    * Ho orientato in modo corretto le normali nello spazio. Nel Vertex Shader ho introdotto la Matrice delle Normali (`mat3(transpose(inverse(model))) * normalize(aPos)`). Questa  operazione fa si che la normale punti sempre verso l'esterno in modo corretto, indipendentemente dalle trasformazioni del pianeta.
2. **Il sole veniva illuminato a metà:** Essendo posizionato a coordinate (0,0,0) esattamente come la fonte di luce, il motore grafico applicava l'algoritmo di Lambert anche al Sole, oscurandone delle parti.
    * Ho aggiunto la variabile `uniform bool isSun`. Dal codice C++, comunichiamo alla GPU quando stiamo disegnando la stella (`1`) e quando i pianeti (`0`). Se l'oggetto è il Sole, lo shader salta il calcolo delle ombre e restituisce il colore puro al 100%.
3. **Codice difficile da leggere:** Rendereizzare 8 pianeti copiando a mano i blocchi di matrici `orbit` e `draw` per ognuno mi ha creato un codice difficile da leggere.
    * L'utilizzo del `std::vector<Planet>` unito a un singolo ciclo `for` ha automatizzato il Grafo di Scena. La matrice `orbit` e `draw` vengono ricalcolate per ogni iterazione prendendo i dati dalla struttura. Ho agganciato la luna inserendo un semplice `if (p == 2)` per intercettare l'iterazione della Terra e agganciarne la matrice.

## Comandi 
* **Mouse e tasto sinistra:** permette una rotazione della telecamera;
* **Rotellina Mouse:** zoom in/zoom out;
* **Tasto L:** permette di attivare la visualizzazione Wireframe  mostrando quindi solo le linee della sfera;
* **Tasto F:** permette di attivare la visualizzazione solid, mostrando quindi solo le facce piene della sfera.

Rispetto alle tappe precedenti ho aggiunto un solo comando, e gli altri pianeti, riporto di seguito un video della struttura semi-completa:

![Animazione Sistema Solare](tappa7.gif)

