#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <iostream>

//Vertex Shader
const char* vertexShaderSource = R"(
    #version 410 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 proj;
    void main() {
        gl_Position = proj * view * model * vec4(aPos, 1.0);
    }
)";

//Fragment shader
const char* fragmentShaderSource = R"(
    #version 410 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0, 0.8, 0.0, 1.0);
    }
)";

// Funzione per generare la sfera
void generateSphere(float radius, unsigned int rings, unsigned int sectors, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float const R = 1.0f / (float)(rings - 1);
    float const S = 1.0f / (float)(sectors - 1);
    for (unsigned int r = 0; r < rings; ++r) {
        for (unsigned int s = 0; r < sectors; ++s) {
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

int main() {
    //Impostazioni per OpenGL 4.1 Core Profile
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 4;
    settings.minorVersion = 1;
    settings.attributeFlags = sf::ContextSettings::Core;

    sf::Window window(sf::VideoMode({800, 600}), "Sistema Solare - Tappa 1", sf::State::Windowed, settings);
    window.setVerticalSyncEnabled(true);    //Vsync
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction));
    glEnable(GL_DEPTH_TEST);    // Attivazione dello Z-Buffer

    //Compilazione delle Shader
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //Generazione della sfera
    std::vector<float> v; 
    std::vector<unsigned int> i;
    generateSphere(1.0f, 30, 30, v, i);

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
    float theta = 0.0f; // Elevazione
    float phi = 0.0f;   // Azimuth
    float rho = 5.0f;   // Distanza

    sf::Vector2i lastMousePos;  //Variabile per tracciare il movimento del mouse
    bool isMousePressed = false;

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()){ 
                window.close();
            }

            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {   //Caso click mouse per spostare l'inquadratura
                if (mouseEvent->button == sf::Mouse::Button::Left) {
                    isMousePressed = true;
                    lastMousePos = sf::Mouse::getPosition(window);
                }
            }
            if (event->is<sf::Event::MouseButtonReleased>()) isMousePressed = false;
        }

        // Logica Camera Orbitante
        if (isMousePressed) {
            sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
            sf::Vector2i delta = currentMousePos - lastMousePos;
            phi += delta.x * 0.01f;
            theta += delta.y * 0.01f;
            // Limite per evitare il Gimbal Lock
            if (theta > 1.5f) theta = 1.5f;
            if (theta < -1.5f) theta = -1.5f;
            lastMousePos = currentMousePos;
        }

        // Calcolo posizione camera da coordinate sferiche a cartesiane
        float camX = rho * std::cos(theta) * std::sin(phi);
        float camY = rho * std::sin(theta);
        float camZ = rho * std::cos(theta) * std::cos(phi);

        //Creazione delle matrici di trasformazione
        glm::mat4 view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0,0,0), glm::vec3(0,1,0)); //cosa guarda, dove guarda e dove è l'alto
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);    //Campo visivo, aspect ratio, piano vicino e piano lontano
        glm::mat4 model = glm::mat4(1.0f); // Sfera al centro

        // Rendering
        glClearColor(0.05f, 0.05f, 0.1f, 1.0f); // Spazio scuro
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Mando le matrici calcolate allo shader
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, i.size(), GL_UNSIGNED_INT, 0);

        window.display();
    }
    return 0;
}