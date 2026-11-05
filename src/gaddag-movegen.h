#ifndef DSA_PROJECT_2_SCRABBLE_GADDAG_MOVEGEN_H
#define DSA_PROJECT_2_SCRABBLE_GADDAG_MOVEGEN_H

#include <string>
#include "board.h"
#include "gaddag.h"
using namespace std;

struct Move {
    string word;
    int startRow;
    int startCol;
    bool horizontal; // true if word is horizontal, false if move is vertical
};

string printMove(Move& move); // command for debugging and for help in CLI
string printMove(Move& move, int score); // command for debugging and for help in CLI
vector<Move> generateMovesGADDAG(Board& board, string& rack, ScrabbleGaddag& dict);
// helper commands will go here :D
vector<Move> generateMovesAtSquare(Board& board, string& rack, ScrabbleGaddag& dict, Position pos);
void Searcher(Board& board, std::unordered_multiset<char>& partialTiles, std::string partialWord, GaddagNode* node, Position currPos, vector<Move>& moves, bool horizontal, bool forward);
bool crossChecksPass(Board& board, char tile, Position place, bool horizontal);
bool addMove(Board& board, std::string partialWord, Position currPos, vector<Move>& moves, bool horizontal, bool forward); // BEFORE calling next level, if TERMINAL and the tiles after the back and front are NOT OCCUPIED, add move!
#endif
