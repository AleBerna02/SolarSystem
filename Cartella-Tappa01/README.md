# Tappa 1: Inizializzazione della finestra e creazione della sfera
Il progetto è stato ideato da **Bernardello Alessio (S5592038)**, studente dell'università di Genova.


In questa prima tappa del progetto mi sono concentrato principalmente su quattro aspetti principali:
1. **Il primo** è la creazione della finestra tipica di SFML:

 ```cpp
sf::Window window(sf::VideoMode({800, 600}), "Sistema Solare - Tappa 1", sf::State::Windowed, settings);
    window.setVerticalSyncEnabled(true);
 ```
Con il **primo comando** ho creato una finestra grande 800x600, il cui titolo è "Sistema solare - Tappa 1".

Con il **secondo comando** invece ho fatto in modo che i frame per secondo venissero limitati a quelli supportati dallo schermo in cui viene eseguito il programma (questo è uno dei problemi riscontrati che riporterò dopo).

Sempre riguardo alla configurazione della finestra ho anche utilizzato:
 ```cpp
 sf::ContextSettings
 ```
 Per fare in modo di utilizzare con sicurezza OpenGL 4.1 Core Profile, ho utilizzato il Core Profile in modo che le funzioni deprecate della fixed-function pipeline forzando l'uso della pipeline programmabile moderna 

2. Il **secondo aspetto** sulla quale mi sono concentrato è stata la generazione della mesh, non ho importato un modello 3D esterno in quanto creandola posso modificare la risoluzione della sfera tramite i parametri "rings" e "sectors" della funzione  ```generateSphere ``` che ora riporto di seguito:

 ```cpp
 void generateSphere(float radius, unsigned int rings, unsigned int sectors, 
    std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);

    for (unsigned int r = 0; r < rings; ++r) {
        for (unsigned int s = 0; r < sectors; ++s) { // Nota: Correzione loop s
            if(s >= sectors) break; 
            float y = std::sin(-M_PI_2 + M_PI * r * R);
            float x = std::cos(2 * M_PI * s * S) * std::sin(M_PI * r * R);
            float z = std::sin(2 * M_PI * s * S) * std::sin(M_PI * r * R);
            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);
        }
    }

    for (unsigned int r = 0; r < rings - 1; ++r) {
        for (unsigned int s = 0; s < sectors - 1; ++s) {
            indices.push_back(r * sectors + s);
            indices.push_back(r * sectors + (s + 1));
            indices.push_back((r + 1) * sectors + (s + 1));
            indices.push_back((r + 1) * sectors + (s + 1));
            indices.push_back((r + 1) * sectors + s);
            indices.push_back(r * sectors + s);
        }
    }
}
```
Possiamo dividere questa funzione in due parti:
* La prima è definita dai primi due cicli che servono a scorrere la sfera come se fosse una griglia di paralleli  e meridiani, poi applicando le formule delle coordinate sferiche (utilizzando sin e cos) converte gli angoli in posizioni cartesiane spaziali (ovvero X,Y,Z) e le salva negli array dei vertici;
* La seconda parte è definita dagli altri due cicli, questi servono a generare un elenco di indici che unisce i punti adiacenti a gruppi di tre, ogni blocco della griglia viene formato unendo due triangoli.

3. Il **terzo aspetto** è la scrittura e la compilazione del:
    * **Vertex Shader:** Questo riceve i vertici nello spazio locale ed applica le matrici Model, View e Projection per trasformarli nello spazio di clip, simulando sia la prospettiva che la camera
    * **Fragment Shader:** In questa parte in realtà colora soltanto di giallo i pixel del "Sole".

4. Il **quarto aspetto** è stato l'utilizzo del polling degli eventi di SFML (```std::optional<sf::Event>```) ho implementato il controllo del mouse trascinando il cursoe per il movimento della telecamera. Trascinando il cursore vengono modificati gli angoli "theta" e "phi", questi angoli vengono poi convertiti in coordinate cartesiane per generare la matrice di vista, cosi facendo sono riuscito a creare una telecamera che gira perfettamente intorno all'origine del mondo.

## Problemi incontrati e le loro soluzioni

Tre sono stati i problemi che ho riscontrato:
1. **L'ottimizzazione della memoria e la duplicazione dei vertici:** Se avessi inviato alla GPU ogni singolo angolo che va a comporre la scheda avrei avuto una duplicazioni degli angoli (tipica della metodologia per salvare i vertici denominata Polygon Soup)
    * Per risolvere questo problema ho utilizzato l'architettura ad indici creando due buffer, il **Vertex Buffer Object** che contiene solo i vertici, e **l'Element Buffer Object** che contiene un array di indici interi. Il tutot viene salvato nel **Vertex Array Object**
2. **Il ribaltamento della telecamera:** Essendo che in questa fase del progetto ho lasciato la libertà all'utente di muoveri senza restrizioni, la telecamera poteva superare i poli della sfera, quando questo succedeva la telecamera di capovolgeva invertendo i comandi 
    * Per risolvere questo problema ho semplicemente messo dei limiti sull'angolo verticale (theta), dando come limite un valore compreso tra -1.5 e 1.5 radianti, cosi facendo la telecamera si ferma poco prima di raggiungere il polo nord o su del sistema.
3. **La quantità di FPS generati:** Quando andavo ad aprire il programma la mia GPU lavorava al massimo, rischiando un surriscaldamento enorme di essa.
    *Per risolvere (come già citato nel punto 1 ad inizio file) ho implementato il comando ```window.setVerticalSyncEnabled(true);``` che permette di abbassare il carico di lavoro della GPU andando a generare un numero di FPS pari al massimo che lo schermo può supportare.

## Lista dei comandi e video dimostrativo
L'unico comando, al momento, presente nel progettoè lo spostamento con il mouse, tenendo premuto il pulsante sinistro del mouse e spostando quest'ultimo è possibile ruoteare la telecamera per visionare la sfera generata.

Di seguito riporto un video che mostra l'implementazione totale di quento spiegato fino ad adesso, con anche il movimento della sfera:

![Animazione Sistema Solare](tappa1.gif)