#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <iostream>

//Vertex Shader che ora passa la posizione locale del vertice al fragment shader
const char* vertexShaderSource = R"(
    #version 410 core
    layout (location = 0) in vec3 aPos;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;
    out vec3 vPos;

    void main() {
        vPos = aPos; // Salvo la posizione del vertice
        gl_Position = proj * view * model * vec4(aPos, 1.0);
    }
)";

//Ora il fragment shader calcola l'illuminazione di base
const char* fragmentShaderSource = R"(
    #version 410 core
    out vec4 FragColor;
    
    in vec3 vPos;

    void main() {
        //Calcolo la distanza dal centro della sfera
        float distanceFromCenter = length(vPos.xy);
        

        float shading = 1.0 - (distanceFromCenter * 0.6); 
        
        // Applica l'ombreggiatura al colore giallo base
        FragColor = vec4(1.0, 0.8, 0.0, 1.0) * shading; 
    }
)";


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

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 4;
    settings.minorVersion = 1;
    settings.attributeFlags = sf::ContextSettings::Core;
    unsigned int winWidth = 800;
    unsigned int winHeight = 600;

    sf::Window window(sf::VideoMode({winWidth, winHeight}), "Sistema Solare - Tappa 02", sf::State::Windowed, settings);
    window.setVerticalSyncEnabled(true);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction));
    glEnable(GL_DEPTH_TEST);

    // Compilazione Shader
    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    // Generazione Sfera
    std::vector<float> v; std::vector<unsigned int> i;
    generateSphere(1.0f, 40, 40, v, i); //Aumentata la risoluzione della sfera da 30x30 a 40x40

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i.size() * sizeof(unsigned int), i.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Parametri Camera Orbitante
    float theta = 0.0f; 
    float phi = 0.0f;   
    float rho = 4.0f;   //Zoom più vicino

    sf::Vector2i lastMousePos;
    bool isMousePressed = false;

    // Stampa le istruzioni nella console
    std::cout << "--- COMANDI TAPPA 02 ---" << std::endl;
    std::cout << "Mouse Sinistro: Ruota la telecamera orbitante" << std::endl;
    std::cout << "Tasto 'L': Modalita' Wireframe (Mesh visibile)" << std::endl;
    std::cout << "Tasto 'F': Modalita' Solida (Fill)" << std::endl;

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()){
                window.close();
            }

            if (const auto* resizeEvent = event->getIf<sf::Event::Resized>()) { //Caso resize della finestra
                // Aggiornamento delle dimensioni dlla finestra per adattarsi alla nuova dimensione
                winWidth = resizeEvent->size.x;
                winHeight = resizeEvent->size.y;
    
                //Aggiornamento del viewport
                glViewport(0, 0, winWidth, winHeight);
            }   

            // Gestione Mouse
            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    isMousePressed = true;
                    lastMousePos = sf::Mouse::getPosition(window);
                }
            }

            if (event->is<sf::Event::MouseButtonReleased>()){ 
                isMousePressed = false;
            }
        }

        //Caso pulsante premuto
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::L)) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::F)) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Solido
        }

        // Logica Camera Orbitante
        if (isMousePressed) {
            sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
            sf::Vector2i delta = currentMousePos - lastMousePos;
            phi += delta.x * 0.01f;
            theta += delta.y * 0.01f;
            if (theta > 1.5f) theta = 1.5f;
            if (theta < -1.5f) theta = -1.5f;
            lastMousePos = currentMousePos;
        }

        float camX = rho * std::cos(theta) * std::sin(phi);
        float camY = rho * std::sin(theta);
        float camZ = rho * std::cos(theta) * std::cos(phi);

        glm::mat4 view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0,0,0), glm::vec3(0,1,0));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)winWidth / (float)winHeight, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f); 

        glClearColor(0.02f, 0.02f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, i.size(), GL_UNSIGNED_INT, 0);

        window.display();
    }
    return 0;
}