#ifndef DSA_PROJECT_2_SCRABBLE_GADDAG_MOVEGEN_H
#define DSA_PROJECT_2_SCRABBLE_GADDAG_MOVEGEN_H
#include <unordered_set>
#include "move.h"

vector<Move> generateMovesGADDAG(const Board& board, const string& rack, ScrabbleGaddag& dict);
vector<Move> generateMovesAtSquare(const Board& board, const string& rack,ScrabbleGaddag& dict, Position pos);
void Searcher(const Board& board, std::unordered_multiset<char>& partialTiles, std::string partialWord, GaddagNode* node, GaddagNode* root, Position currPos, vector<Move>& moves, bool horizontal, bool forward);
bool crossChecksPass(const Board& board, char tile, GaddagNode* root, Position place, bool horizontal);
bool addMove(const Board& board, std::string partialWord, Position currPos, vector<Move>& moves, bool horizontal, bool forward);

#endif