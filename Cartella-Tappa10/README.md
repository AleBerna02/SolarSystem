# Tappa 10
Il progetto è stato ideato da **Bernardello Alessio (S5592038)**, studente dell'università di Genova.

In questa tappa ho voluto modificare ed aggiungere alcuni aspetti per rendere la visualizzazione del sistema solare più scorrevole, modificando la telecamera rendendola word in Hand, ho introdotto la luce speculare, il rendering dell'anello di saturno ed il tracciamento delle orbite.

In questa tappa ho aggiunto/modificato le seguenti cose:
1. **Telecamera Free-Fly:** ho completamente modificato la telecamera vincolata al centro. Al suo posto ho introdotto una telecamera libera di muoversi nello spazio, usando `camPos` (per la posizione), `camfront` (per la direzione dello sguardo) e `camUp` (per l'orientamento verso l'alto). Inoltre ora il mouse calcola il Pitch e lo Yaw.
2. **Luce speculare:** Adesso oltre alla luce diffusa, il Fragment Shader ora calcola anche il riflesso speculare basandosi sulla posizione attuale della telecamera (grazie a `viewPos`), creando un "punto di luce" sugli oceani o sulle superifici lisce dei pianeti;
3. **Orbita ed Anelli planetari:** Ho aggiunto due funzioni generatrici:
    * `generateOrbit`: crea un cerchio di vertici renderizzati come `GL_LINE_LOOP` tramite un terzo Shader Program dedicato alle linee;
    * `generatering`: crea un disco 2D mappato con coordinate UV per simulare l'anello di Saturno;
4. **Alpha Blending:** ho attivato il `GL_BLEND` di OpernGL per permettere il rendering delle sfumature trasparenti che servono per le linee delle orbite e per l'anello di Saturno.
5. **Modifica delle orbite:** Prima i pianeti giravano sempplicemente intorno al sole tutti alla stessa distanza l'uno dall'altro, adesos ognuno ha una orbita a distanza diversa per renderlo più fedele alla realtà.

## Problemi e soluzioni

1. **Gimbal Lock:** Guardando troppo in alto o troppo in basso con il mouse, la telecamera si capovolgeva improvvisamente, invertendo i controlli.
    * Per risolvere ho bloccato il Pitch, mettendo due controlli (`if (pitch > 89.0f) pitch = 89.0f;` e `if (pitch < -89.0f) pitch = -89.0f;`), ho impedito che la telecamera superasse i 90 gradi;
2. **Anello di Saturno invisibile dal basso:** L'anello di Saturno è un disco piatto bidimensionale, guardandolo da sotto esso scompariva totalmente. questo a colpa di OpenGL che, per risparmiare risorse, non renderizza i triangoli che rivolgono la schiena alla telecamera;
    * Prima di renderizzare i pianeti e gli anelli ho disabilitato momentaneamente l'eliminazione delle facce posteriori con `glDisable(GL_CULL_FACE)`. Cosi facnedo la GPU renderizza i triangoli dell'anello da entrambe le direzioni, rendendolo visibile indipendentemente dall'angolo di inquadratura. Le facce vengono riattivate a fine ciclo.
3. **Texture degli anelli come sfondo nero:** Nonostante l'immagine fosse un `.png` trasparente, OpenGL disegnava un quadrato nero attorno al disco;
    * Utilizzando `glEnable(GL_BLEND)` e specificando l'equazione `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`, ho detot alla scheda video di usare il canale alpha andando a fondere l'anello con il colore dello sfondo preesistente.

## Comandi

Con il cambio della telecamera anche i comandi hanno subuto un enorme cambiamento, riporto quindi di seguito tutti i comandi del progetto terminato:

* **Mouse e tasto sinistra:** permette una rotazione della telecamera;
* **Rotellina Mouse:** zoom in/zoom out;
* **Tasto L:** permette di attivare la visualizzazione Wireframe  mostrando quindi solo le linee della sfera;
* **Tasto F:** permette di attivare la visualizzazione solid, mostrando quindi solo le facce piene della sfera;
* **Tasto P:** permette di fermare la rotazione dei pianeti;
* **Tasto W:** vai avanti;
* **Tasto S:** vai indietro;
* **Tasto A:** vai a sinistra;
* **Barra spaziatrice:** sali;
* **Tasto W:** scendi;

Riporto di seguito un video di tutta la tappa completa con utilizzo di tutti i comandi sopra riportati:

Purtroppo il file .gif che mostra il progetto completo occupa troppo, di conseguenza non mi è stato possibile pubblicarlo. Invito a visionare la tappa 10 per visionarlo nella sua interezza completa.



