#include <unordered_set>
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
vector<Move> generateMovesGADDAG(Board& board, string& rack, ScrabbleGaddag& dict) {
    // outer layer 
    // for every square - if anchor then find all moves hooking it*
    // *to avoid double-counting moves playing on multiple anchors, we consider only the left-most/top-most anchor
    vector<Move> moves;
    for (int row = 0; row < board.Size; row++) {
        for (int col = 0; col < board.Size; col++) {
            if (board.isAnchor(Position{row, col})) {
                vector<Move> anchorMoves = generateMovesAtSquare(board, rack, dict, Position{row, col});
                for (Move move : anchorMoves) {
                    moves.push_back(move);
                }
            }
        }
    }
    return moves;
}

vector<Move> generateMovesAtSquare(Board& board, string& rack, ScrabbleGaddag& dict, Position pos) {
    vector<Move> moves;
    std::unordered_multiset<char> tiles;
    for (char c : rack) {
        tiles.insert(c);
    }
    // vertical
    verticalSearcher(board, tiles, dict.getRoot(), pos, moves);
    // horizontal
    horizontalSearcher(board, tiles, dict.getRoot(), pos, moves);
    return moves;
}

void verticalSearcher(Board& board, std::unordered_multiset<char> partialTiles, GaddagNode* node, Position currPos, vector<Move>& moves);
void horizontalSearcher(Board& board, std::unordered_multiset<char> partialTiles, GaddagNode* node, Position currPos, vector<Move>& moves);
bool crossChecksPass(Board& board, char tile, Position place, bool horizontal);