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
    verticalSearcher(board, tiles, "", dict.getRoot(), pos, moves, false);
    // horizontal
    horizontalSearcher(board, tiles, "", dict.getRoot(), pos, moves, false);
    return moves;
}

void verticalSearcher(Board& board, std::unordered_multiset<char>& partialTiles, std::string partialWord, GaddagNode* node, Position currPos, vector<Move>& moves, bool forward) {
    if (partialWord == "") {
        // first call
        // check all tiles A - Z
        for (int i = 0; i < 26; i++) {
            char tile = 'A' + i;
            if (partialTiles.count(tile) != 0 && crossChecksPass(board, tile, currPos, false)) {
                // place move
                auto tile_it = partialTiles.find(tile);
                partialTiles.erase(tile_it);
                currPos.row--;
                // recursive call
                verticalSearcher(board, partialTiles, (partialWord + tile), node->children[i], currPos, moves, forward);
                // undo changes
                partialTiles.insert(tile);
                currPos.row++;
            }
        }
        // check blank [no need to do reverse yet]
    }
    else {
        // Are we on an occupied tile? (Yes -> add to gaddag if available, else break; No -> continue)
        // Are we on the board? (No -> if top, move to bottom along with gaddag if available, else break (including if bottom) Yes -> continue)
        // Are we on an anchor? (Yes -> if in reverse phase, go to bottom along with gaddag if available, else break (words with multiple anchors count one) No -> continue)
        // For all possible placements that are 1) in rack or optionally go to bottom 2) in gaddag 3) survive cross checks - place square and call next level
        // BEFORE calling next level, if TERMINAL and the tile after the back is NOT OCCUPIED, add move!
    }
}
void horizontalSearcher(Board& board, std::unordered_multiset<char>& partialTiles, std::string partialWord, GaddagNode* node, Position currPos, vector<Move>& moves, bool forward);
bool crossChecksPass(Board& board, char tile, Position place, bool horizontal);