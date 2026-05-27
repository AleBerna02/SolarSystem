# Sistema Solare 3D - OpenGL

Il progetto è stato ideato da **Bernardello Alessio (S5592038)**, studente dell'università di Genova.


Il progetto che ho fatto implementa una simulazione tridimensionale di un sistema solare in C++ e OpenGL. Il progetto include la generazione delle mesh per i vari pianeti, la mappatura delle texture anche tramite mipmapping, una skybox per lo sfondo stellato, un modello di calcolo dell'illuminazione basato sull'equazione di Blinn-Phong e l'implementazione di un grafo di scena.

## Compilazione e Build del progetto

Il progetto utilizza il file CMake per compilare. Per generare i file di configurazione e compilare l'eseguibile, eseguire i seguenti comandi dal terminale all'interno della root del progetto:

```bash
# 1. Creazione della cartella di build con spostamento in essa (bisogna spostarsi all'interno della cartella "Solar System" prima di tutto)
mkdir build
cd build

# 2. Configurazione del progetto
cmake ..

# 3. Compilazione del progetto 
cmake --build .

```

Dopo il completamento della compilazione bisogna seguire i seguenti passaggi, partendo da dentro la cartella "build":

```bash

# Spostarsi all'interno della cartella Debug che contiene i file eseguibili
cd Debug

# Per verificare il funzionamento corretto dei file eseguibili utilizzare il comando

.\Tappa01.exe (per Windows)
./Tappa01.exe (per Mac)

Per cambiare la tappa che si vuole visualizzare basta cambiare il numero scegliendo tra 01, 02, 03, 04, 05, 06, 07, 08, 09, 10.
```


## Comandi

Nel mio progetto fino alla tappa 10 vengono utilizzati seguenti comandi:

* **Trascinamento del mouse:** Permette di spostare la visuale;
* **Pulsante L:** Per visionare la griglia delle sfere;
* **Pulsante F:** Per visionare le sfere ripiene del loro colore.

Nella tappa 8, oltre ai comandi precedenti ho aggiunto un solo comando extra:
* **Pulsante P:** Permette di mettere in pausa il sistema solare


A partire dalla tappa 10 ho implementato la camera-in-hand, quindi sono stati aggiunti dei comandi (riporto di seguito tutti i comandi anche i precedenti):

* **Trascinamento del mouse:** Permette di spostare la visuale;
* **Pulsante L:** Per visionare la griglia delle sfere;
* **Pulsante F:** Per visionare i puaneti con la texture completa;
* **Pulsante P:** Permette di mettere in pausa il sistema solare;
* **Pulsante W:** Permette di spostarsi in avanti rispetto alla posizione della telecamera;
* **Pulsante S:** Permette di spostarsi ll'indietro rispetto alla posizione della telecamera;
* **Pulsante A:** Permette di spostarsi a sinistra rispetto alla posizione della telecamera;
* **Pulsante D:** Permette di spostarsi a destra rispetto alla posizione della telecamera;
* **Barra spaziatrice:** Permette di spostarsi verso l'alto;
* **Pulsante Shift:** Permette di spostarsi verso il basso.
