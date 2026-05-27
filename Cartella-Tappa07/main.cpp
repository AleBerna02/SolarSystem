#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp> 
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <cmath>
#include <iostream>

const char* vertexShaderSource = R"(
    #version 410 core
    layout (location = 0) in vec3 aPos;
    
    uniform mat4 model; 
    uniform mat4 view;  
    uniform mat4 proj;  
    
    out vec3 FragPos; 
    out vec3 Normal;  

    void main() {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * normalize(aPos);
        gl_Position = proj * view * vec4(FragPos, 1.0); //Calcola la posizione finale di ogni vertice
    }
)";

const char* fragmentShaderSource = R"(
    #version 410 core
    out vec4 FragColor;
    
    in vec3 FragPos;
    in vec3 Normal;
    
    uniform vec3 objectColor;
    uniform vec3 lightPos; 
    uniform bool isSun;    

    void main() {
        if(isSun) {
            FragColor = vec4(objectColor, 1.0);
        } else {
            float ambientStrength = 0.05;
            vec3 ambient = ambientStrength * objectColor;
            
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos); 
            
            float diff = max(dot(norm, lightDir), 0.0); 
            vec3 diffuse = diff * objectColor; 
            
            vec3 result = ambient + diffuse;
            FragColor = vec4(result, 1.0);
        }
    }
)";

void generateSphere(float radius, unsigned int rings, unsigned int sectors, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    float const R = 1.0f / (float)(rings - 1); float const S = 1.0f / (float)(sectors - 1);
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

//Struct per tutti i pianetri
struct Planet {
    float distance;      
    float size;          
    float orbitSpeed;    
    float rotationSpeed; 
    glm::vec3 color;    //Aggiunto per dare un colore diverso a tutti i pianeti 
};

int main() {
    sf::ContextSettings settings;
    settings.depthBits = 24;
    settings.majorVersion = 4;
    settings.minorVersion = 1;
    settings.attributeFlags = sf::ContextSettings::Core;
    unsigned int winWidth = 800;
    unsigned int winHeight = 600;

    sf::Window window(sf::VideoMode({winWidth, winHeight}), "Sistema Solare - Tappa 07", sf::State::Windowed, settings);
    window.setVerticalSyncEnabled(true);
    gladLoadGLLoader(reinterpret_cast<GLADloadproc>(sf::Context::getFunction));
    glEnable(GL_DEPTH_TEST); 

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

    std::vector<float> v;
    std::vector<unsigned int> i;
    generateSphere(1.0f, 40, 40, v, i); 

    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, i.size() * sizeof(unsigned int), i.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);

    //Vettore per i pianeti indicano, distanza dal sole, dimensione, velocità di orbita, velocità di rotazione, colore
    std::vector<Planet> solarSystem = {
        { 2.0f,  0.15f, 1.5f,  1.0f,  glm::vec3(0.5f, 0.5f, 0.5f) }, // Mercurio 
        { 3.2f,  0.28f, 1.1f, -0.5f,  glm::vec3(0.9f, 0.7f, 0.3f) }, // Venere
        { 4.5f,  0.30f, 0.8f,  2.0f,  glm::vec3(0.2f, 0.4f, 0.9f) }, // Terra
        { 6.0f,  0.18f, 0.6f,  1.9f,  glm::vec3(0.8f, 0.3f, 0.2f) }, // Marte
        { 9.5f,  0.80f, 0.3f,  4.0f,  glm::vec3(0.7f, 0.6f, 0.5f) }, // Giove
        { 13.0f, 0.65f, 0.2f,  3.8f,  glm::vec3(0.9f, 0.8f, 0.6f) }, // Saturno
        { 17.0f, 0.45f, 0.1f, -2.5f,  glm::vec3(0.4f, 0.7f, 0.9f) }, // Urano
        { 21.0f, 0.42f, 0.08f, 2.6f,  glm::vec3(0.1f, 0.2f, 0.8f) }  // Nettuno
    };

    float theta = 0.5f; 
    float phi = 0.0f;
    float rho = 15.0f; 
    sf::Vector2i lastMousePos;
    bool isMousePressed = false;
    sf::Clock clock; 

    //Serve ad indicare da quale punto arriva la luce, in questo caso dal centro dov'è posizionato il sole
    glUniform3f(glGetUniformLocation(program, "lightPos"), 0.0f, 0.0f, 0.0f);

    std::cout << "--- Comandi Tappa 07 ---" << std::endl;
    std::cout << "Mouse Sinistro: Ruota la telecamera orbitante" << std::endl;
    std::cout << "Tasto 'L': Modalita' Wireframe (Mesh visibile)" << std::endl;
    std::cout << "Tasto 'F': Modalita' Solida (Fill)" << std::endl;
    std::cout << "Rotellina: Zoom In/Out" << std::endl;
    
    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();

        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()){
                window.close();
            }

            if (const auto* resizeEvent = event->getIf<sf::Event::Resized>()) {
                winWidth = resizeEvent->size.x; 
                winHeight = resizeEvent->size.y; 
                glViewport(0, 0, winWidth, winHeight);
            }

            if (const auto* mouseEvent = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouseEvent->button == sf::Mouse::Button::Left) { isMousePressed = true; lastMousePos = sf::Mouse::getPosition(window); }
            }
            if (event->is<sf::Event::MouseButtonReleased>()){
                isMousePressed = false;
            }
            
            // Gestione Input Mouse per lo zoom
            if (const auto* scrollEvent = event->getIf<sf::Event::MouseWheelScrolled>()) {
                if (scrollEvent->wheel == sf::Mouse::Wheel::Vertical) {
                    rho -= scrollEvent->delta * 1.5f; 
                    if (rho < 2.0f) rho = 2.0f;       
                    if (rho > 80.0f) rho = 80.0f;     
                }
            }

            if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code == sf::Keyboard::Key::L) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                }
                if (keyEvent->code == sf::Keyboard::Key::F) {
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                }
            }
        }

        if (isMousePressed) {
            sf::Vector2i currentMousePos = sf::Mouse::getPosition(window); sf::Vector2i delta = currentMousePos - lastMousePos;
            phi += delta.x * 0.01f; theta += delta.y * 0.01f;
            if (theta > 1.5f){
                theta = 1.5f;
            }
            if (theta < -1.5f) {
                theta = -1.5f;
            }
            lastMousePos = currentMousePos;
        }

        float camX = rho * std::cos(theta) * std::sin(phi);
        float camY = rho * std::sin(theta);
        float camZ = rho * std::cos(theta) * std::cos(phi);
        glm::mat4 view = glm::lookAt(glm::vec3(camX, camY, camZ), glm::vec3(0,0,0), glm::vec3(0,1,0));

        float aspectRatio = (float)winWidth / (float)winHeight;
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

        glClearColor(0.01f, 0.01f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(program, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
        glBindVertexArray(VAO);

        //Sole
        glm::mat4 modelSun = glm::mat4(1.0f); 
        modelSun = glm::rotate(modelSun, time * 0.2f, glm::vec3(0.0f, 1.0f, 0.0f)); 
        glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(modelSun));
        glUniform3f(glGetUniformLocation(program, "objectColor"), 1.0f, 0.8f, 0.0f); 
        glUniform1i(glGetUniformLocation(program, "isSun"), 1); 
        glDrawElements(GL_TRIANGLES, i.size(), GL_UNSIGNED_INT, 0);

        //Scorro il vettore dei pianeti in modo da disegnarli tutti senza dover scrivere righe di codice ripetute inutilmente
        for (int p = 0; p < solarSystem.size(); ++p) {
            glm::mat4 orbit = glm::mat4(1.0f);
            orbit = glm::rotate(orbit, time * solarSystem[p].orbitSpeed, glm::vec3(0.0f, 1.0f, 0.0f)); 
            orbit = glm::translate(orbit, glm::vec3(solarSystem[p].distance, 0.0f, 0.0f)); 

            glm::mat4 draw = orbit;
            draw = glm::rotate(draw, time * solarSystem[p].rotationSpeed, glm::vec3(0.0f, 1.0f, 0.0f));
            draw = glm::scale(draw, glm::vec3(solarSystem[p].size)); 
            
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(draw));
            glUniform3f(glGetUniformLocation(program, "objectColor"), solarSystem[p].color.r, solarSystem[p].color.g, solarSystem[p].color.b); 
            glUniform1i(glGetUniformLocation(program, "isSun"), 0); 
            glDrawElements(GL_TRIANGLES, i.size(), GL_UNSIGNED_INT, 0);

            //Seleziona la Terra per agganciare la Luna
            if (p == 2) {
                glm::mat4 moonDraw = orbit; 
                moonDraw = glm::rotate(moonDraw, time * 1.5f, glm::vec3(0.0f, 1.0f, 0.0f)); 
                moonDraw = glm::translate(moonDraw, glm::vec3(0.8f, 0.0f, 0.0f)); 
                moonDraw = glm::scale(moonDraw, glm::vec3(0.08f)); 
                glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, glm::value_ptr(moonDraw));
                glUniform3f(glGetUniformLocation(program, "objectColor"), 0.6f, 0.6f, 0.6f); 
                glDrawElements(GL_TRIANGLES, i.size(), GL_UNSIGNED_INT, 0);
            }
        }

        window.display();
    }
    return 0;
}