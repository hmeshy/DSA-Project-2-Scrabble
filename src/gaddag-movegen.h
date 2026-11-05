#ifndef DSA_PROJECT_2_SCRABBLE_GADDAG_MOVEGEN_H
#define DSA_PROJECT_2_SCRABBLE_GADDAG_MOVEGEN_H

#include "move.h"

vector<Move> generateMovesGADDAG(const Board& board, const string& rack, ScrabbleGaddag& dict);
vector<Move> generateMovesAtSquare(const Board& board, const string& rack,ScrabbleGaddag& dict, Position pos);

#endif
