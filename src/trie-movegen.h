#ifndef SCRABBLE_TRIE_MOVEGEN_H
#define SCRABBLE_TRIE_MOVEGEN_H

#include "move.h"

vector<Move> generateMovesTrie(const Board& board, const string& rack, const ScrabbleTrie& dict);

#endif
