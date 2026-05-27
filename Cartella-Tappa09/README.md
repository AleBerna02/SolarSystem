# Tappa 9
Il progetto è stato ideato da **Bernardello Alessio (S5592038)**, studente dell'università di Genova.

In questa tappa ho deciso di aggiungere un ulteriore livello di dettaglio andando a creare la skybox per aggiungere le stelle come "sfondo" per i pianeti.

In questa tappa ho aggiunto/modificato le seguenti cose:

1. **Doppio shader:** Ho utilizzato due shader dedicati soltanto allo sfondo (`skyboxVS` e `skyboxFs`), cosi facendo ho un rendering separato da quello dei pianeti evitandomi possibili conflitti.
2. **Texture cubemap:** Invece di usare una sola immagine ho creato la funzione `loadCubemap` che carica un array contenente 6 immagini, queste vengono inviate alla GPU come un'unica texture a 6 dimensioni;
3. **Geometria del cubo:** Ho aggiunto un nuovo VBO/VAO che contiene 36 vertici (ovvero per tutte le coordinate 3D) di un cubo, all'interno di esso è presente ora la telecamera.

## Problemi riscontrati e soluzioni
1. **Uscita dal cubo della telecamera:** Muovendosi con la telecamera (zommando oppure dezoomando), si poteva rischiare di uscire dal "cubo stellato" facendo cosi sembrare che lo spazio non fosse effettivamente infinito.
    * Per risolvere adesso lo Skybox segue la telecamera all'infinito facendo in modo che quest'ultima non possa mai uscire dai bordi prestabiliti;
2. **La skybox copriva i pianeti:** Essendo il cubo che ho creato molto grande, se veniva renderizzato normalmente esso avrebbe coperto la geometria di tutto il sistema solare;
    * Per risolvere ho:
        *Forzato la profonditaà di ogni vertice dello Skybox impostando il loro valore al numero più alto possibile (`1.0`)
        * Prima di disegnare lo Skybox, ho cambiato la funzione di profondità di `GL_LEQUAL`. siccome il Depth Buffer viene impostato ad 1, lo Skybox (che vale 1 anche quello) supera il test e viene disegnato solo dove non ci sono già dei pianeti (che ovviamente devono avere una profondità <`1.0`) piazzandosi cosi come sfondo.

## Comandi
Rispetto alla tappa precedente non ho aggiunto nessuno comando di conseguenza rimangono invariati. Li riporto comunque di seguito:

* **Mouse e tasto sinistra:** permette una rotazione della telecamera;
* **Rotellina Mouse:** zoom in/zoom out;
* **Tasto L:** permette di attivare la visualizzazione Wireframe  mostrando quindi solo le linee della sfera;
* **Tasto F:** permette di attivare la visualizzazione solid, mostrando quindi solo le facce piene della sfera;
* **Tasto P:** permette di fermare la rotazione dei pianeti.

Di seguito riporto un video che mostra tutte le funzionalità aggiunte con questa tappa:

![Animazione Sistema Solare](tappa9.gif)