#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp> 
#include <SFML/Graphics/Image.hpp> 
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

/*Questo è il vertex shader dei pianeti, questo include:
    -  fragPos: Serve a calcolare la posizione del vertice nel mondo 3D;
    -  normal: Serve ad adattare le normali per l'illuminazione;
    -   TexCoord: Passa le coordinate UV della texture al livello dopo;
    -   gl_Position: Proietta il vertice 3D sullo schermo 2D, utilizzando le matrici model, view e proj.
*/
const char* planetVS = R"(
    #version 410 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec2 aTexCoord; 
    uniform mat4 model; 
    uniform mat4 view; 
    uniform mat4 proj;  
    out vec3 FragPos; 
    out vec3 Normal; 
    out vec2 TexCoord; 
    void main() {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * normalize(aPos);
        TexCoord = aTexCoord;
        gl_Position = proj * view * vec4(FragPos, 1.0);
    }
)";

/*Questo è il fragment shader dei pianeti, include:
    -  texture1: La texture del pianeta;
    -  lightPos: La posizione della luce (il sole);
    -  viewPos: La posizione della telecamera, necessaria per il calcolo dello speculare;
    -  isSun: Un booleano che indica se il pianeta è il sole, in modo da applicare un'illuminazione diversa.

Inoltre lo shader calcola l'illuminazione finale a seguito dell'utilizzo dell'equazione di illuminazione di Phong
*/
const char* planetFS = R"(
    #version 410 core
    out vec4 FragColor;
    in vec3 FragPos; 
    in vec3 Normal; 
    in vec2 TexCoord; 
    
    uniform sampler2D texture1; 
    uniform vec3 lightPos; 
    uniform vec3 viewPos;
    uniform bool isSun;    
    
    void main() {
        vec4 texColor = texture(texture1, TexCoord);
        
        float sunBrightness = 1.5;      
        float ambientLight = 0.25;      
        float diffuseBrightness = 1.4;  
        float specularStrength = 0.5;   
        
        if(isSun) { 
            vec3 brightSun = clamp(texColor.rgb * sunBrightness, 0.0, 1.0);
            FragColor = vec4(brightSun, texColor.a); 
        } else {
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos); 
            vec3 viewDir = normalize(viewPos - FragPos);   
            
            // Diffusa
            float diff = max(dot(norm, lightDir), 0.0);
            
            // Speculare
            vec3 halfwayDir = normalize(lightDir + viewDir);  
            float spec = pow(max(dot(norm, halfwayDir), 0.0), 16.0);
            
            vec3 result = (ambientLight + (diff * diffuseBrightness)) * texColor.rgb + (spec * specularStrength); 
            FragColor = vec4(clamp(result, 0.0, 1.0), texColor.a);
        }
    }
)";

//Shader delle orbite
const char* lineVS = R"(
    #version 410 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model; 
    uniform mat4 view; 
    uniform mat4 proj;
    void main() { 
    gl_Position = proj * view * model * vec4(aPos, 1.0); 
    }
)";
const char* lineFS = R"(
    #version 410 core
    out vec4 FragColor;
    void main() { 
    FragColor = vec4(0.5, 0.5, 0.5, 0.3); 
    } 
)";

//Shader dello skybox
const char* skyboxVS = R"(
    #version 410 core
    layout (location = 0) in vec3 aPos;
    out vec3 TexCoords;
    uniform mat4 view; 
    uniform mat4 proj;
    void main() {
        TexCoords = aPos;
        vec4 pos = proj * view * vec4(aPos, 1.0);
        gl_Position = pos.xyww; 
    }
)";
const char* skyboxFS = R"(
    #version 410 core
    out vec4 FragColor;
    in vec3 TexCoords;
    uniform samplerCube skybox;
    void main() { 
    FragColor = texture(skybox, TexCoords); 
    }
)";

/*La funzione generateSphere crea i vertici e gli indici per generare una sfera, in particolare:
    - radius: Definisce la grandezza della sfera;
    - rings e sectors: Permettono di decidere la risoluzione delle mesh.
    - verices: vettore passato alla funzione che verrà popolato con i dati di ogni punto della sfera (posizione e coordinate UV);
    - indices: secondo vettore passato alla funzione, questo conterrà le informazioni per sapere come i punti generati devono essere collegati tra di loro per formare i triangoli.
    
    Il primo ciclo utilizza sin e cos per posizionare i vari punti nello spazio e inizia già a mappare le coordinate UV da 0 a 1, 
    il secondo ciclo invece si occupa di generare gli indici per formare i triangoli, collegando i punti in modo ordinato.
*/
void generateSphere(float radius, unsigned int rings, unsigned int sectors, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float const R = 1.0f / (float)(rings - 1); 
    float const S = 1.0f / (float)(sectors - 1);
    for (unsigned int r = 0; r < rings; ++r) {
        for (unsigned int s = 0; s < sectors; ++s) {
            float y = std::sin(-M_PI_2 + M_PI * r * R); 
            float x = std::cos(2 * M_PI * s * S) * std::sin(M_PI * r * R); 
            float z = std::sin(2 * M_PI * s * S) * std::sin(M_PI * r * R);
            vertices.push_back(x * radius); 
            vertices.push_back(y * radius); 
            vertices.push_back(z * radius);
            vertices.push_back((float)s / (sectors - 1)); 
            vertices.push_back((float)r / (rings - 1)); 
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

/*La funzione genera un disco piatto con un buco al centro per creare l'anello di saturno, in particolare:
    - innerRadius: Raggio interno del disco, che definisce la grandezza del buco al centro;
    - outerRadius: Raggio esterno del disco, che definisce la grandezza complessiva dell'anello;
    - steps: Numero di segmenti utilizzati, più è alto più è rotondo l'anello;
    - vertices: Vettore passato alla funzione che verrà popolato con i dati di ogni punto dell'anello (posizione e coordinate UV).
    
*/
void generateRing(float innerRadius, float outerRadius, int steps, std::vector<float>& vertices) {
    for (int i = 0; i <= steps; i++) {
        float angle = 2.0f * M_PI * (float)i / (float)steps;
        float x = cos(angle); 
        float z = sin(angle);
        vertices.push_back(x * innerRadius); 
        vertices.push_back(0.0f); 
        vertices.push_back(z * innerRadius);
        vertices.push_back(0.0f); 
        vertices.push_back(0.5f); 
        vertices.push_back(x * outerRadius); 
        vertices.push_back(0.0f); 
        vertices.push_back(z * outerRadius);
        vertices.push_back(1.0f); 
        vertices.push_back(0.5f);
    }
}
//Funzione per generare i vertici dell'orbita
void generateOrbit(float radius, int steps, std::vector<float>& vertices) {
    for (int i = 0; i < steps; i++) {
        float angle = 2.0f * M_PI * (float)i / (float)steps;
        vertices.push_back(cos(angle) * radius); 
        vertices.push_back(0.0f); 
        vertices.push_back(sin(angle) * radius);
    }
}

//funzione che verrà richiamata dopo per caricare le texture dei pianeti, crea un identificativo per ogni texture e lo restituisce
unsigned int loadTexture(const char* path) {
    sf::Image img; if (!img.loadFromFile(path)) return 0;
    img.flipVertically(); 
    unsigned int texID; 
    glGenTextures(1, &texID); 
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
    glGenerateMipmap(GL_TEXTURE_2D); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return texID;
}

//funzione per caricare la skybox, è simile alla precedente ma utilizza GL_TEXTURE_CUBE_MAP e carica 6 texture diverse per ogni faccia del cubo
unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for (unsigned int i = 0; i < faces.size(); i++) {
        sf::Image img;
        if (img.loadFromFile(faces[i])) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());
        } else {
            std::cout << "Skybox immagine mancante: " << faces[i] << std::endl;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}

//Struct dei pianeti
struct Planet { 
    float realDistance; //Distanza del pianeta dal sole
    float realRadius;   //Raggio del pianeta
    float orbitSpeed;   //Velocità di orbita perchè ogni pianeta ha una velocità diversa
    float rotationSpeed;    //Velocità di rotazione su se stesso
    float axialTilt;    //Inclinazione dell'asse per rendere realistica l'orbita
    unsigned int textureID;     //Id della texture 
    bool hasRing;   //Se ha un anello o meno, per Saturno
};

int main() {
    sf::ContextSettings settings; 
    settings.depthBits = 24; 
    settings.majorVersion = 4; 
    settings.minorVersion = 1; 
    settings.attributeFlags = sf::ContextSettings::Core;
    unsigned int winWidth = 800;    //salvate perchè dopo servono per la modifica della grandezza della finestra
    unsigned int winHeight = 600;
    sf::Window window(sf::VideoMode({winWidth, winHeight}), "Sistema Solare - Tappa 10", sf::State::Windowed, settings);    //Creo la finestra
    window.setVerticalSyncEnabled(true);    //VSync
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction));
    
    glEnable(GL_DEPTH_TEST); 
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Compilazione delle shader, creo un programma per ogni shader (pianeti, linee e skybox) e gli collego i rispettivi vertex e fragment shader
    unsigned int vsP = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vsP, 1, &planetVS, NULL); 
    glCompileShader(vsP);
    unsigned int fsP = glCreateShader(GL_FRAGMENT_SHADER); 
    glShaderSource(fsP, 1, &planetFS, NULL); 
    glCompileShader(fsP);
    unsigned int planetProgram = glCreateProgram(); 
    glAttachShader(planetProgram, vsP); 
    glAttachShader(planetProgram, fsP); 
    glLinkProgram(planetProgram);

    unsigned int vsL = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vsL, 1, &lineVS, NULL); 
    glCompileShader(vsL);
    unsigned int fsL = glCreateShader(GL_FRAGMENT_SHADER); 
    glShaderSource(fsL, 1, &lineFS, NULL); 
    glCompileShader(fsL);
    unsigned int lineProgram = glCreateProgram(); 
    glAttachShader(lineProgram, vsL); 
    glAttachShader(lineProgram, fsL); 
    glLinkProgram(lineProgram);

    unsigned int vsS = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vsS, 1, &skyboxVS, NULL); 
    glCompileShader(vsS);
    unsigned int fsS = glCreateShader(GL_FRAGMENT_SHADER); 
    glShaderSource(fsS, 1, &skyboxFS, NULL); 
    glCompileShader(fsS);
    unsigned int skyboxProgram = glCreateProgram(); 
    glAttachShader(skyboxProgram, vsS); 
    glAttachShader(skyboxProgram, fsS); 
    glLinkProgram(skyboxProgram);


    std::vector<float> v; 
    std::vector<unsigned int> i; 
    generateSphere(1.0f, 50, 50, v, i); 
    unsigned int planetVAO, VBO, EBO; 
    //Vado a salvare le informazioni all'interno del Vertex Buffer shader ed all'interno dell'index buffer oobject invece salvo gli indici ai vertici
    glGenVertexArrays(1, &planetVAO); 
    glGenBuffers(1, &VBO); 
    glGenBuffers(1, &EBO);
    glBindVertexArray(planetVAO); 
    glBindBuffer(GL_ARRAY_BUFFER, VBO); 
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); 
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i.size() * sizeof(unsigned int), i.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);

    //Faccio la stessa cosa per l'anello
    std::vector<float> rv; 
    generateRing(1.2f, 2.2f, 64, rv);
    unsigned int ringVAO, ringVBO; 
    glGenVertexArrays(1, &ringVAO); 
    glGenBuffers(1, &ringVBO);
    glBindVertexArray(ringVAO); 
    glBindBuffer(GL_ARRAY_BUFFER, ringVBO);
    glBufferData(GL_ARRAY_BUFFER, rv.size() * sizeof(float), rv.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);

    //Faccio la stessa cosa per l'orbita, questa volta però non ho bisogno di un index buffer object perchè disegno le orbite con GL_LINE_LOOP che disegna una linea continua collegando tutti i vertici in ordine, quindi basta solo il vertex buffer object
    std::vector<float> ov; 
    generateOrbit(1.0f, 128, ov);
    unsigned int orbitVAO, orbitVBO; 
    glGenVertexArrays(1, &orbitVAO); 
    glGenBuffers(1, &orbitVBO);
    glBindVertexArray(orbitVAO); 
    glBindBuffer(GL_ARRAY_BUFFER, orbitVBO);
    glBufferData(GL_ARRAY_BUFFER, ov.size() * sizeof(float), ov.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);

    //Mesh per lo skybox è un cubo con vertici che vanno da -1 a 1, in questo modo quando viene scalato con la matrice di proiezione e vista, sembra sempre lontano e avvolge tutta la scena
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f
    };
    
    unsigned int skyboxVAO, skyboxVBO; 
    glGenVertexArrays(1, &skyboxVAO); 
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO); 
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);

    //Sezione dedicata al caricamento delle texture utilizzando la funzione creata prima, ogni pianeta ha la sua texture e viene salvata in una variabile dedicata al pianeta
    std::cout << "Caricamento Risorse..." << std::endl;
    unsigned int texSun = loadTexture("../Cartella-risorse/sole.jpg");
    unsigned int texMercury = loadTexture("../Cartella-risorse/mercurio.jpg"); 
    unsigned int texVenus = loadTexture("../Cartella-risorse/venere.jpg");
    unsigned int texEarth = loadTexture("../Cartella-risorse/terra.jpg");     
    unsigned int texMoon = loadTexture("../Cartella-risorse/luna.jpg");
    unsigned int texMars = loadTexture("../Cartella-risorse/marte.jpg");       
    unsigned int texJupiter = loadTexture("../Cartella-risorse/giove.jpg");
    unsigned int texSaturn = loadTexture("../Cartella-risorse/saturno.jpg");   
    unsigned int texUranus = loadTexture("../Cartella-risorse/urano.jpg");
    unsigned int texNeptune = loadTexture("../Cartella-risorse/nettuno.jpg");
    unsigned int texRing = loadTexture("../Cartella-risorse/saturno_anello.png");

    std::vector<std::string> faces = {
        "../Cartella-risorse/stelle.jpg", "../Cartella-risorse/stelle.jpg", 
        "../Cartella-risorse/stelle.jpg", "../Cartella-risorse/stelle.jpg", 
        "../Cartella-risorse/stelle.jpg", "../Cartella-risorse/stelle.jpg"
    };

    unsigned int cubemapTexture = loadCubemap(faces);

    //realDistance      realRadius    orbitSpeed  rotationSpeed  axialTilt   textureID   hasRing
    std::vector<Planet> solarSystem = {
        { 0.39f,  0.38f,  4.1f,   0.017f,   0.03f, texMercury, false }, 
        { 0.72f,  0.95f,  1.6f,  -0.004f, 177.30f, texVenus,   false }, 
        { 1.00f,  1.00f,  1.0f,   1.000f,  23.40f, texEarth,   false },   
        { 1.52f,  0.53f,  0.5f,   0.970f,  25.10f, texMars,    false },
        { 5.20f, 11.20f,  0.08f,  2.400f,   3.10f, texJupiter, false }, 
        { 9.58f,  9.45f,  0.03f,  2.200f,  26.70f, texSaturn,  true  }, 
        { 19.2f,  4.00f,  0.01f, -1.400f,  97.70f, texUranus,  false }, 
        { 30.0f,  3.88f,  0.006f, 1.500f,  28.30f, texNeptune, false }
    };

    // Sezione dedicata alla telecamera libera
    glm::vec3 camPos   = glm::vec3(0.0f, 8.0f, 25.0f);  //Posizione fisica della telecamera
    glm::vec3 camFront = glm::vec3(0.0f, 0.0f, -1.0f);  //Direzione verso cui la telecamera sta guardando
    glm::vec3 camUp    = glm::vec3(0.0f, 1.0f,  0.0f);  //Vettore che indica qual è la direzione "verso l'alto" per la telecamera
    
    float yaw   = -90.0f;   //Rotazione attorno all'asse Y (in gradi). Inizialmente impostato a -90 per far guardare la telecamera verso il centro
    float pitch = -15.0f;   //Rotazione attorno all'asse X (in gradi). Inizialmente leggermente inclinato verso il basso

    //salvataggio della posizione del mouse per calcolare la differenza di movimento e aggiornare l'orientamento della telecamera
    sf::Vector2i lastMousePos; 
    bool isMousePressed = false; 
    sf::Clock clock; //Variabile per il tempo, mi serve per rendere il movimento indipendente dal framerate
    float accumulatedTime = 0.0f; 
    bool isPaused = false;  //Serve per mettere in pausa il movimento dei pianeti

    std::cout << "--- Comandi Tappa 10 ---" << std::endl;
    std::cout << "Mouse (Tieni premuto): Guardati intorno" << std::endl;
    std::cout << "W / S: Avanti / Indietro" << std::endl;
    std::cout << "A / D: Sinistra / Destra" << std::endl;
    std::cout << "Spazio / Shift: Sali / Scendi" << std::endl;

    //Da qui in poi è il ciclo principale del programma, che continua finché la finestra è aperta
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();  //Calcolo del tempo trascorso dall'ultimo frame
        if (!isPaused) accumulatedTime += deltaTime;

        //Sezione dedicata alla gestione degli eventi
        while (const std::optional<sf::Event> event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()){    //evento per la chiusura della finestra
                window.close();
            }
            
            if (const auto* resizeEvent = event->getIf<sf::Event::Resized>()) { //Evento per il resize della finestra che aggiorna il viewport
                winWidth = resizeEvent->size.x; 
                winHeight = resizeEvent->size.y; 
                glViewport(0, 0, winWidth, winHeight);
            }
            
            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {   //Evento per la pressione del mouse, se è il tasto sinistro attivo la rotazione della telecamera e salvo la posizione iniziale del mouse
                if (mouseEvent->button == sf::Mouse::Button::Left) { 
                    isMousePressed = true; 
                    lastMousePos = sf::Mouse::getPosition(window); 
                }
            }
            
            if (event->is<sf::Event::MouseButtonReleased>()){ 
                isMousePressed = false;
            }

            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {     //Evento per la pressione dei tasti
                if (keyEvent->code == sf::Keyboard::Key::P){    //Se si preme P mette in pausa o toglie la pausa al movimento dei pianeti
                    isPaused = !isPaused; 
                }
                if (keyEvent->code == sf::Keyboard::Key::L){    //Se si preme L attiva la modalità wireframe
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                if (keyEvent->code == sf::Keyboard::Key::F) {   // Se si preme F torna alla modalità di rendering normale
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }
        }

        
        if (isMousePressed) {
            sf::Vector2i currentMousePos = sf::Mouse::getPosition(window); 
            sf::Vector2i delta = currentMousePos - lastMousePos;    //Calcolo di quanti pixel si è spostato il mouse rispetto alla posizione precedente
            
            float sensitivity = 0.15f;  //Sensibilità del mouse
            yaw += delta.x * sensitivity; //Aggiorno lo yaw in base allo spostamento orizzontale del mouse
            pitch -= delta.y * sensitivity; //Aggiorno il pitch in base allo spostamento verticale del mouse, ho invertito per farlo sembrare più normale
            
            
            if (pitch > 89.0f){ //Cap per evitare il capovolgimento
                 pitch = 89.0f; 
            }
            if (pitch < -89.0f) {
                pitch = -89.0f;
            }
            lastMousePos = currentMousePos;     //Cambio la poisizione del mouse precedente altrimenti tornerbbe sempre alla posizione iniziale
        }

        // Aggiorniamo il vettore di direzione della telecamera
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        camFront = glm::normalize(front);

        // --- MOVIMENTO TASTIERA (Volo) ---
        float cameraSpeed = 15.0f * deltaTime; // Velocità della navicella
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) camPos += cameraSpeed * camFront; //Sposta in avanti
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) camPos -= cameraSpeed * camFront; //sposta indietro
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) camPos -= glm::normalize(glm::cross(camFront, camUp)) * cameraSpeed;  //sposta a sinistra 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) camPos += glm::normalize(glm::cross(camFront, camUp)) * cameraSpeed;  //sposta a destra
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) camPos += cameraSpeed * camUp;    //Sali
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) camPos -= cameraSpeed * camUp;   //Scendi

        // Calcoliamo la nuova matrice View usando la posizione libera
        glm::mat4 view = glm::lookAt(camPos, camPos + camFront, camUp);
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)winWidth / (float)winHeight, 0.1f, 100.0f);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        //Vado a preparare le orbite con un calcolo logaritmo per rendere più visibili anche quelle vicine al sole come consigliato dal prof
        glUseProgram(lineProgram);
        glUniformMatrix4fv(glGetUniformLocation(lineProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(lineProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glBindVertexArray(orbitVAO);
        
        //Scorro il vettore dei pianeti per disegnare ogni orbita
        for(int p = 0; p < solarSystem.size(); ++p) {
            float scaledDistance = log(solarSystem[p].realDistance + 1.0f) * 8.0f;
            glm::mat4 mOrbit = glm::scale(glm::mat4(1.0f), glm::vec3(scaledDistance));
            glUniformMatrix4fv(glGetUniformLocation(lineProgram, "model"), 1, GL_FALSE, glm::value_ptr(mOrbit));
            glDrawArrays(GL_LINE_LOOP, 0, 128); 
        }

        //Preparazione dei pianeti e del sole
        glDisable(GL_CULL_FACE);
        glActiveTexture(GL_TEXTURE0); 
        glUseProgram(planetProgram);
        glUniform1i(glGetUniformLocation(planetProgram, "texture1"), 0);
        glUniformMatrix4fv(glGetUniformLocation(planetProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(planetProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        
        glUniform3f(glGetUniformLocation(planetProgram, "lightPos"), 0.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(planetProgram, "viewPos"), camPos.x, camPos.y, camPos.z); 
        
        glBindVertexArray(planetVAO);
        
        //Sole
        glBindTexture(GL_TEXTURE_2D, texSun); 
        glm::mat4 modelSun = glm::mat4(1.0f); 
        modelSun = glm::rotate(modelSun, accumulatedTime * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f)); 
        glUniformMatrix4fv(glGetUniformLocation(planetProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelSun));
        glUniform1i(glGetUniformLocation(planetProgram, "isSun"), 1); 
        glDrawElements(GL_TRIANGLES, i.size(), GL_UNSIGNED_INT, 0);

        //ciclo per i pianeti che scorre ancora il vettore dei pianeti, questa volta però con un calcolo più complesso per posizionarli correttamente in base alla loro distanza, velocità di rotazione e inclinazione
        glUniform1i(glGetUniformLocation(planetProgram, "isSun"), 0); 
        for (int p = 0; p < solarSystem.size(); ++p) {
            
            float scaledDistance = log(solarSystem[p].realDistance + 1.0f) * 8.0f;
            float scaledSize     = log(solarSystem[p].realRadius + 1.0f) * 0.4f;

            glm::mat4 orbit = glm::mat4(1.0f); 
            orbit = glm::rotate(orbit, accumulatedTime * solarSystem[p].orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f)); 
            orbit = glm::translate(orbit, glm::vec3(scaledDistance, 0.0f, 0.0f)); 
            
            glm::mat4 draw = orbit; 
            draw = glm::rotate(draw, glm::radians(solarSystem[p].axialTilt), glm::vec3(0.0f, 0.0f, 1.0f)); 
            draw = glm::rotate(draw, accumulatedTime * solarSystem[p].rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f)); 
            draw = glm::scale(draw, glm::vec3(scaledSize)); 
            
            glBindVertexArray(planetVAO);
            glBindTexture(GL_TEXTURE_2D, solarSystem[p].textureID);
            glUniformMatrix4fv(glGetUniformLocation(planetProgram, "model"), 1, GL_FALSE, glm::value_ptr(draw));
            glDrawElements(GL_TRIANGLES, i.size(), GL_UNSIGNED_INT, 0);

            //Caso della luna che deve essere figlia della terra, quindi prendo la matrice di trasformazione della terra e ci applico sopra le trasformazioni per posizionare la luna correttamente in orbita attorno alla terra
            if (p == 2) {
                glBindTexture(GL_TEXTURE_2D, texMoon); 
                glm::mat4 moonDraw = orbit; 
                moonDraw = glm::rotate(moonDraw, accumulatedTime * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f)); 
                moonDraw = glm::translate(moonDraw, glm::vec3(0.8f, 0.0f, 0.0f)); 
                moonDraw = glm::scale(moonDraw, glm::vec3(scaledSize * 0.27f)); 
                glUniformMatrix4fv(glGetUniformLocation(planetProgram, "model"), 1, GL_FALSE, glm::value_ptr(moonDraw));
                glDrawElements(GL_TRIANGLES, i.size(), GL_UNSIGNED_INT, 0);
            }

            //Quando trovo il pianeta con l'anello lo disegno dopo aver disegnato il pianeta, in questo modo l'anello viene disegnato sopra il pianeta e non rischia di essere nascosto da esso
            if(solarSystem[p].hasRing) {
                glBindVertexArray(ringVAO);
                glBindTexture(GL_TEXTURE_2D, texRing);
                
                glm::mat4 ringModel = orbit; 
                ringModel = glm::rotate(ringModel, glm::radians(solarSystem[p].axialTilt), glm::vec3(0.0f, 0.0f, 1.0f)); 
                ringModel = glm::scale(ringModel, glm::vec3(scaledSize));
                
                glUniformMatrix4fv(glGetUniformLocation(planetProgram, "model"), 1, GL_FALSE, glm::value_ptr(ringModel));
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 130);
                glEnable(GL_CULL_FACE);
            }
        }

        //Skybox
        glDepthFunc(GL_LEQUAL);  
        glUseProgram(skyboxProgram);

        //rimuovo la traslazione dalla matrice di vista per far sembrare lo skybox infinito
        glm::mat4 viewSkybox = glm::mat4(glm::mat3(view)); 
        glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "view"), 1, GL_FALSE, glm::value_ptr(viewSkybox));
        glUniformMatrix4fv(glGetUniformLocation(skyboxProgram, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE1); 
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glUniform1i(glGetUniformLocation(skyboxProgram, "skybox"), 1); 

        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS); 

        window.display();
    }
    return 0;
}