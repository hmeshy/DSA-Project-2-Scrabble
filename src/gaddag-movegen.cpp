#include <unordered_set>
#include "gaddag-movegen.h"
using namespace std; 
string printMove(Move& move) {
    // [ROW][COL] MOVE for horizontal, [COL][ROW] MOVE for vertical
    // regular letters are UPPERCASE, blanks are lowercase; already in row
    string val;
    if (move.horizontal) {
        val += to_string(move.startRow+1);
        val += ('A' + move.startCol); 
    }
    else {
        val += ('A' + move.startCol);
        val += to_string(move.startRow+1); 
    }
    val += " " + move.word;
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
    Searcher(board, tiles, "", dict.getRoot(), dict.getRoot(), pos, moves, false, false);
    // horizontal
    Searcher(board, tiles, "", dict.getRoot(), dict.getRoot(), pos, moves, true, false);
    return moves;
}

void Searcher(Board& board, std::unordered_multiset<char>& partialTiles, std::string partialWord, GaddagNode* node, GaddagNode* root, Position currPos, vector<Move>& moves, bool horizontal, bool forward) {
    if (partialWord == "") {
        // first call
        // check all tiles A - Z
        for (int i = 0; i < 26; i++) {
            char tile = 'A' + i;
            if (partialTiles.count(tile) != 0 && node->children[i] && crossChecksPass(board, tile, root, currPos, horizontal)) {
                // place move
                auto tile_it = partialTiles.find(tile);
                partialTiles.erase(tile_it);
                if (horizontal) {
                    currPos.column--;
                }
                else {
                    currPos.row--;
                }
                // recursive call
                Searcher(board, partialTiles, (partialWord + tile), node->children[i], root, currPos, moves, horizontal, forward);
                // undo changes
                partialTiles.insert(tile);
                if (horizontal) {
                    currPos.column++;
                }
                else {
                    currPos.row++;
                }
            }
        }
        // check blank [no need to do reverse at root]
        if (partialTiles.count('?') != 0) {
            for (int i = 0; i < 26; i++) {
                char letter = 'a' + i;
                if (node->children[i] && crossChecksPass(board, letter, root, currPos, horizontal)) {
                    auto tile_it = partialTiles.find('?');
                    partialTiles.erase(tile_it);
                    if (horizontal) {
                        currPos.column--;
                    }
                    else {
                        currPos.row--;
                    }
                    // recursive call
                    Searcher(board, partialTiles, (partialWord + letter), node->children[i], root, currPos, moves, horizontal, forward);
                    // undo changes
                    partialTiles.insert('?');
                    if (horizontal) {
                        currPos.column++;
                    }
                    else {
                        currPos.row++;
                    }
                }
            }
        }
    }
    else {
        // Are we on an occupied tile? (Yes -> add to gaddag if available, else break; No -> continue)
        if (board.isOccupied(currPos)) {
            char letter = board.getLetter(currPos);
            int letterIndex = letter - 'A';
            if (letterIndex > 25) { // adjust index for blanks
                letterIndex = letter - 'a';
            }
            if (node->children[letterIndex]) {
                if (forward) {
                    std::string nextWord = partialWord + letter;
                    GaddagNode* nextNode = node->children[letterIndex];
                    if (nextNode->terminal) {
                        addMove(board, nextWord, currPos, moves, horizontal, forward);
                    }
                    if (horizontal) {
                        currPos.column++;
                    }
                    else {
                        currPos.row++;
                    }
                    Searcher(board, partialTiles, nextWord, nextNode, root, currPos, moves, horizontal, forward);
                    if (horizontal) {
                        currPos.column--;
                    }
                    else {
                        currPos.row--;
                    }
                }
                else {
                    std::string nextWord = letter + partialWord;
                    GaddagNode* nextNode = node->children[letterIndex];
                    if (nextNode->terminal) {
                        addMove(board, nextWord, currPos, moves, horizontal, forward);
                    }
                    if (horizontal) {
                        currPos.column--;
                    }
                    else {
                        currPos.row--;
                    }
                    Searcher(board, partialTiles, nextWord, nextNode, root, currPos, moves, horizontal, forward);
                    if (horizontal) {
                        currPos.column++;
                    }
                    else {
                        currPos.row++;
                    }
                }
            }
        }
        // Are we on the board? (No -> if top, move to bottom along with gaddag if available, else break (including if bottom) Yes -> continue)
        // Are we on an anchor? (If yes and reverse, must go to other side!; if yes and regular, need cross checks!)
        else if ( ((currPos.row == -1 || currPos.column == -1) || (!forward && board.isAnchor(currPos)))) { 
            if ((node->children[26])) { // make this inside the else if, so the next else if does not run!
                if (horizontal) {
                    currPos.column += (partialWord.size() + 1);
                }
                else {
                    currPos.row += (partialWord.size() + 1);
                }
                Searcher(board, partialTiles, partialWord, node->children[26], root, currPos, moves, horizontal, true);
                if (horizontal) {
                    currPos.column -= (partialWord.size() + 1);
                }
                else {
                    currPos.row -= (partialWord.size() + 1);
                }
            }
        }
        else if (currPos.row < 15 && currPos.column < 15) { // not beyond board
            // unconstrainted part of board - check all nonblanks, blanks, and reverse
            bool isAnchor = board.isAnchor(currPos);
            // regular tiles
            for (int i = 0; i < 26; i++) {
                char tile = 'A' + i;
                if (partialTiles.count(tile) != 0 && node->children[i] && (!isAnchor || crossChecksPass(board, tile, root, currPos, horizontal))) {
                    // place move
                    auto tile_it = partialTiles.find(tile);
                    partialTiles.erase(tile_it);
                    GaddagNode* nextNode = node->children[i];
                    if (forward) {
                        string nextWord = (partialWord + tile);
                        if (nextNode->terminal) {
                            addMove(board, nextWord, currPos, moves, horizontal, forward);
                        }
                        if (horizontal) {
                            currPos.column += 1;
                        }
                        else {
                            currPos.row += 1;
                        }
                        // recursive call
                        Searcher(board, partialTiles, nextWord, nextNode, root, currPos, moves, horizontal, forward);
                        // undo changes 
                        if (horizontal) {
                            currPos.column -= 1;
                        }
                        else {
                            currPos.row -= 1;
                        }
                    }
                    else {
                        string nextWord = (tile + partialWord);
                        if (nextNode->terminal) {
                            addMove(board, nextWord, currPos, moves, horizontal, forward);
                        }
                        if (horizontal) {
                            currPos.column -= 1;
                        }
                        else {
                            currPos.row -= 1;
                        }
                        // recursive call
                        Searcher(board, partialTiles, nextWord, nextNode, root, currPos, moves, horizontal, forward);
                        // undo changes 
                        if (horizontal) {
                            currPos.column += 1;
                        }
                        else {
                            currPos.row += 1;
                        }
                    }
                    partialTiles.insert(tile);
                }
            }
            // blanks
            if (partialTiles.count('?') != 0) {
                for (int i = 0; i < 26; i++) {
                    char letter = 'a' + i;
                    if (node->children[i] && (!isAnchor || crossChecksPass(board, letter, root, currPos, horizontal))) {
                        // place move
                        auto tile_it = partialTiles.find('?');
                        partialTiles.erase(tile_it);
                        GaddagNode* nextNode = node->children[i];
                        if (forward) {
                            string nextWord = (partialWord + letter);
                            if (nextNode->terminal) {
                                addMove(board, nextWord, currPos, moves, horizontal, forward);
                            }
                            if (horizontal) {
                                currPos.column += 1;
                            }
                            else {
                                currPos.row += 1;
                            }
                            // recursive call
                            Searcher(board, partialTiles, nextWord, nextNode, root, currPos, moves, horizontal, forward);
                            // undo changes 
                            if (horizontal) {
                                currPos.column -= 1;
                            }
                            else {
                                currPos.row -= 1;
                            }
                        }
                        else {
                            string nextWord = (letter + partialWord);
                            if (nextNode->terminal) {
                                addMove(board, nextWord, currPos, moves, horizontal, forward);
                            }
                            if (horizontal) {
                                currPos.column -= 1;
                            }
                            else {
                                currPos.row -= 1;
                            }
                            // recursive call
                            Searcher(board, partialTiles, nextWord, nextNode, root, currPos, moves, horizontal, forward);
                            // undo changes 
                            if (horizontal) {
                                currPos.column += 1;
                            }
                            else {
                                currPos.row += 1;
                            }
                        }
                        partialTiles.insert('?');
                    }
                }
            }
            // reverse
            if (node->children[26]) {
                if (horizontal) {
                    currPos.column += (partialWord.size() + 1);
                }
                else {
                    currPos.row += (partialWord.size() + 1);
                }
                Searcher(board, partialTiles, partialWord, node->children[26], root, currPos, moves, horizontal, true);
                if (horizontal) {
                    currPos.column -= (partialWord.size() + 1);
                }
                else {
                    currPos.row -= (partialWord.size() + 1);
                }
            }
        }        
    }
}
bool crossChecksPass(Board& board, char tile, GaddagNode* root, Position place, bool horizontal) {
    // if horizontal, make sure the vertical forms a word :)
    if (horizontal) {
        if (!board.isOccupied(Position{place.row-1, place.column}) && !board.isOccupied(Position{place.row+1, place.column})) {
            return true; // no other words made
        }
        Position currPos = place;
        bool topOccupied = false;
        while(board.isOccupied(Position{currPos.row-1, currPos.column})){
            topOccupied = true;
            currPos.row--;
            char placed = board.getLetter(currPos);
            int letterIndex = placed - 'A';
            if (letterIndex > 25) { // adjust index for blanks
                letterIndex = placed - 'a';
            }
            if (!root->children[letterIndex]) {
                return false;
            }
            root = root->children[letterIndex];
        }
        if (topOccupied) {
            // reverse, add current tile
            if(!root->children[26]) {
                return false;
            }
            root = root->children[26];
            int letterIndex = tile - 'A';
            if (letterIndex > 25) { // adjust index for blanks
                letterIndex = tile - 'a';
            }
            if (!root->children[letterIndex]) {
                return false;
            }
            root = root->children[letterIndex];
        }
        else {
            // add current tile, reverse
            int letterIndex = tile - 'A';
            if (letterIndex > 25) { // adjust index for blanks
                letterIndex = tile - 'a';
            }
            if (!root->children[letterIndex]) {
                return false;
            }
            root = root->children[letterIndex];
            if(!root->children[26]) {
                return false;
            }
            root = root->children[26];
        }
        currPos = place;
        while(board.isOccupied(Position{currPos.row+1, currPos.column})){
            currPos.row++;
            char placed = board.getLetter(currPos);
            int letterIndex = placed - 'A';
            if (letterIndex > 25) { // adjust index for blanks
                letterIndex = placed - 'a';
            }
            if (!root->children[letterIndex]) {
                return false;
            }
            root = root->children[letterIndex];
        }
    }
    // if vertical, make sure the horizontal forms a word :)
    else {
        if (!board.isOccupied(Position{place.row, place.column-1}) && !board.isOccupied(Position{place.row, place.column+1})) {
            return true; // no other words made
        }
        Position currPos = place;
        bool leftOccupied = false;
        while(board.isOccupied(Position{currPos.row, currPos.column-1})){
            leftOccupied = true;
            currPos.column--;
            char placed = board.getLetter(currPos);
            int letterIndex = placed - 'A';
            if (letterIndex > 25) { // adjust index for blanks
                letterIndex = placed - 'a';
            }
            if (!root->children[letterIndex]) {
                return false;
            }
            root = root->children[letterIndex];
        }
        if (leftOccupied) {
            // reverse, add current tile
            if(!root->children[26]) {
                return false;
            }
            root = root->children[26];
            int letterIndex = tile - 'A';
            if (letterIndex > 25) { // adjust index for blanks
                letterIndex = tile - 'a';
            }
            if (!root->children[letterIndex]) {
                return false;
            }
            root = root->children[letterIndex];
        }
        else {
            // add current tile, reverse
            int letterIndex = tile - 'A';
            if (letterIndex > 25) { // adjust index for blanks
                letterIndex = tile - 'a';
            }
            if (!root->children[letterIndex]) {
                return false;
            }
            root = root->children[letterIndex];
            if(!root->children[26]) {
                return false;
            }
            root = root->children[26];
        }
        currPos = place;
        while(board.isOccupied(Position{currPos.row, currPos.column+1})){
            currPos.column++;
            char placed = board.getLetter(currPos);
            int letterIndex = placed - 'A';
            if (letterIndex > 25) { // adjust index for blanks
                letterIndex = placed - 'a';
            }
            if (!root->children[letterIndex]) {
                return false;
            }
            root = root->children[letterIndex];
        }
    }
    return root->terminal;
}
bool addMove(Board& board, std::string partialWord, Position currPos, vector<Move>& moves, bool horizontal, bool forward) { // we have a terminal word, so we just need to confirm the other tiles are not occupied
    Position left;
    Position right;
    if (horizontal && forward) {
        // we are going left to right, and on the rightmost square
        left = Position{currPos.row, int(currPos.column-partialWord.size())};
        right = Position{currPos.row, currPos.column + 1};
    }
    else if (horizontal && !forward) {
        // we are going from left to right, and on the leftmost square
        left = Position{currPos.row, currPos.column-1};
        right = Position{currPos.row, int(currPos.column + partialWord.size())};
    }
    else if (!horizontal && forward) {
        // we are going from top to bottom and on the bottommost square
        left = Position{int(currPos.row-partialWord.size()), currPos.column};
        right = Position{currPos.row+1, currPos.column};
    }
    else if (!horizontal && !forward) {
        left = Position{currPos.row-1, currPos.column};
        right = Position{int(currPos.row+partialWord.size()), currPos.column};
    }
    if (board.isOccupied(left) || board.isOccupied(right)) {
        return false;
    }
    // valid move, add it
    Move move;
    if (horizontal) {
        move = Move{partialWord, left.row, left.column + 1, horizontal};
    }
    else {
        move = Move{partialWord, left.row + 1, left.column, horizontal};
    }
    moves.push_back(move);
    return true;
}
