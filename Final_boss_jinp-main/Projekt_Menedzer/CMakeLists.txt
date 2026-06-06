cmake_minimum_required(VERSION 3.10)
project(MenedzerHasel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. NAPRAWA BŁĘDU WERSJI: Wymuszamy na kompilatorze docelową wersję Windows 10
add_compile_definitions(_WIN32_WINNT=0x0A00)

find_package(Threads REQUIRED)

set(SOURCES
        main.cpp
        SzyfratorXOR.cpp
        WalidatorSily.cpp
        MenedzerHasel.cpp
)

add_executable(AplikacjaMenedzer ${SOURCES})

# Podpinamy wątki
target_link_libraries(AplikacjaMenedzer PRIVATE Threads::Threads)

# 2. NAPRAWA SIECI: Jeśli jesteśmy na Windowsie, podpinamy bibliotekę sieciową (Winsock)
if(WIN32)
    target_link_libraries(AplikacjaMenedzer PRIVATE ws2_32)
endif()