#include "gaddag-movegen.h"
using namespace std; 
string printMove(Move& move) {
    // [ROW][COL] MOVE for horizontal, [COL][ROW] MOVE for vertical
    // regular letters are UPPERCASE, blanks are lowercase; already in row
    string val;
    if (move.horizontal) {
        val += ('0' + move.startRow) + ('A' + move.startCol - 1) + " "; 
    }
    else {
        val += ('A' + move.startCol - 1) + ('0' + move.startRow) + " "; 
    }
    val += move.word;
    return val;
}
string printMove(Move& move, int score) {
    return to_string(score) + ": " + printMove(move);
}
vector<Move> generateMovesGADDAG(Board& board, string& rack, ScrabbleGaddag& dict);