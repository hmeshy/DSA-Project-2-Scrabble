#ifndef DSA_PROJECT_2_SCRABBLE_CLI_H
#define DSA_PROJECT_2_SCRABBLE_CLI_H

#include "board.h"
#include "gaddag.h"
#include <istream>
#include <ostream>

void runWord(Board& board, ScrabbleTrie& dict, std::istream& in, std::ostream& out);
void runWordGADDAG(Board& board, ScrabbleTrie& dict, ScrabbleGaddag& gaddag, std::istream& in, std::ostream& out);

#endif
