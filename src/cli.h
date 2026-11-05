#ifndef DSA_PROJECT_2_SCRABBLE_CLI_H
#define DSA_PROJECT_2_SCRABBLE_CLI_H

#include "board.h"
#include "gaddag.h"

#include <istream>
#include <ostream>

void runMenu(ScrabbleTrie& dict, ScrabbleGaddag& gad, std::istream& in, std::ostream& out);

#endif
