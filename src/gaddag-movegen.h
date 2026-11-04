#ifndef DSA_PROJECT_2_SCRABBLE_GADDAG_MOVEGEN_H
#define DSA_PROJECT_2_SCRABBLE_GADDAG_MOVEGEN_H

#include <string>
#include <tuple>
#include "board.h"
#include "gaddag.h"
using namespace std;

struct Move {
    string word;
    int startRow;
    int startCol;
    bool horizontal; // true if word is horizontal, false if move is vertical
};

std::string printMove(Move& move); // command for debugging and for help in CLI
std::string printMove(Move& move, int score); // command for debugging and for help in CLI
vector<Move> generateMovesGADDAG(Board& board, string& rack, ScrabbleGaddag& dict);
// helper commands will go here :D


#endif
