cmake_minimum_required(VERSION 4.0.2)
project(DSA_Project_2_Scrabble)

set(CMAKE_CXX_STANDARD 14)

set(SCRABBLE_SRC
        src/board.cpp
        src/cli.cpp
        src/trie.cpp
        src/gaddag.cpp
        src/move.cpp
        src/trie-movegen.cpp
        src/gaddag-movegen.cpp
        src/game.cpp
)

add_executable(DSA_Project_2_Scrabble src/main.cpp ${SCRABBLE_SRC})