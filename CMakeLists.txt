cmake_minimum_required(VERSION 3.16)
project(EsameGrafica3D)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_definitions(_USE_MATH_DEFINES)

# ==========================================
# SCARICAMENTO AUTOMATICO DELLE LIBRERIE
# ==========================================
include(FetchContent)

# Scarica SFML 3.0.2 da GitHub
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Scarica GLM da GitHub
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
)
FetchContent_MakeAvailable(glm)

# ==========================================
# CONFIGURAZIONE AUTOMATICA DI TUTTE LE TAPPE
# ==========================================

# 1. Definisci una lista con i numeri delle tue cartelle
set(TAPPE 
    "01" "02" "03" "04" "05" 
    "06" "07" "08" "09" "10"
)

# 2. Inizia un ciclo che passa attraverso ogni tappa della lista
foreach(NUM ${TAPPE})
    
    # Crea le variabili col nome dell'eseguibile e della cartella
    set(NOME_ESEGUIBILE "Tappa${NUM}")
    set(CARTELLA "Cartella-Tappa${NUM}")

    # 3. Controlla se la cartella e il main.cpp esistono prima di provare a compilarli
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${CARTELLA}/main.cpp")
        
        # Crea l'eseguibile per questa specifica tappa
        add_executable(${NOME_ESEGUIBILE} 
            ${CARTELLA}/main.cpp 
            ${CARTELLA}/glad.c
        )

        # Associa gli include locali della tappa
        target_include_directories(${NOME_ESEGUIBILE} PRIVATE ${CARTELLA}/include)

        # Collega SFML e GLM scaricati automaticamente
        target_link_libraries(${NOME_ESEGUIBILE} PRIVATE 
            SFML::Graphics 
            SFML::Window 
            SFML::System
            glm::glm-header-only
        )

        # Collega OpenGL in base al sistema operativo
        if(WIN32)
            target_link_libraries(${NOME_ESEGUIBILE} PRIVATE opengl32)
        elseif(APPLE)
            target_link_libraries(${NOME_ESEGUIBILE} PRIVATE "-framework OpenGL")
        else()
            target_link_libraries(${NOME_ESEGUIBILE} PRIVATE GL)
        endif()
        
    endif()
    
endforeach()

# Copia automatica della Cartella-risorse nella cartella di build
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/Cartella-risorse" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")