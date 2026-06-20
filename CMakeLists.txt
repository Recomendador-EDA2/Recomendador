cmake_minimum_required(VERSION 3.10)
project(Recomendador
    VERSION 1.0
    DESCRIPTION "Sistema de Recomendacao em Grafos — EDA2"
    LANGUAGES C
)

# Padrao C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Flags de compilacao
add_compile_options(-Wall -Wextra -O2)

# Fontes e headers
set(SOURCES
    src/main.c
    src/digrafo.c
    src/vocab.c
    src/algoritmos.c
    src/recomendador.c
    src/relatorio.c
)

# Executavel
add_executable(recomendador ${SOURCES})

# Diretorio de headers
target_include_directories(recomendador PRIVATE include)

# Linkagem com libm (math)
target_link_libraries(recomendador m)
