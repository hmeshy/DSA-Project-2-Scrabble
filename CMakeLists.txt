cmake_minimum_required(VERSION 4.1)
project(DSA_Project_2_Scrabble)

set(CMAKE_CXX_STANDARD 14)

add_executable(DSA_Project_2_Scrabble
        src/main.cpp
        src/board.cpp
        src/board.h
        src/cli.cpp
        src/cli.h
        src/trie.cpp
        src/trie.h
)
