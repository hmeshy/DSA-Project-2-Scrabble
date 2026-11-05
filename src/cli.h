#ifndef DSA_PROJECT_2_SCRABBLE_CLI_H
#define DSA_PROJECT_2_SCRABBLE_CLI_H

#include "board.h"

#include <istream>
#include <ostream>

void runWord(Board& board, ScrabbleTrie& dict, std::istream& in, std::ostream& out);

#endif
