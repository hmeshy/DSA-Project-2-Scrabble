#include "gaddag-movegen.h"

namespace {
class GaddagSearch {
    const Board& board;
    const GaddagNode* root;
    MoveChecks checks{};
    int cnt[26];
    vector<Move> moves;
    Position anchor;
    bool horiz;
    int dr;
    int dc;

    void saveMove(const string& word, Position start) {
        Move move;
        move.word = word;
        move.startRow = start.row;
        move.startCol = start.column;
        move.horizontal = horiz;
        moves.push_back(move);
    }

    void extendRight(const GaddagNode* node, Position pos, Position start, const string& word) {
        if (node->terminal && word.length() >= 2 && !board.isOccupied(pos)) {
            saveMove(word, start);
        }
        if (!board.withinBoard(pos)) {
            return;
        }

        Position next{pos.row + dr, pos.column + dc};
        if (board.isOccupied(pos)) {
            char ch = board.getLetter(pos);
            const GaddagNode* child = node->children[ch - 'A'];
            if (child != nullptr) {
                string nextWord = word + ch;
                extendRight(child, next, start, nextWord);
            }
            return;
        }
        for (int idx = 0; idx < 26; idx++) {
            const GaddagNode* child = node->children[idx];
            if (cnt[idx] > 0 && child != nullptr && allowed(checks, pos, idx, horiz)) {
                cnt[idx]--;
                char ch = 'A' + idx;
                string nextWord = word + ch;
                extendRight(child, next, start, nextWord);
                cnt[idx]++;
            }
        }
    }

    void extendLeft(const GaddagNode* node, Position start, const string& word) {
        Position prev{start.row - dr, start.column - dc};
        Position after{anchor.row + dr, anchor.column + dc};
        if (!board.isOccupied(prev)) {
            if (node->terminal && word.length() >= 2 && !board.isOccupied(after)) {
                saveMove(word, start);
            }
            if (node->children[26] != nullptr) {
                // Child 26 separates the reversed left half from the right half.
                extendRight(node->children[26], after, start, word);
            }
        }
        if (!board.withinBoard(prev)) {
            return;
        }

        if (board.isOccupied(prev)) {
            char ch = board.getLetter(prev);
            const GaddagNode* child = node->children[ch - 'A'];
            if (child != nullptr) {
                string nextWord = ch + word;
                extendLeft(child, prev, nextWord);
            }
            return;
        }
        if (checks.cell[prev.row][prev.column].anchor) {
            return;
        }
        for (int idx = 0; idx < 26; idx++) {
            const GaddagNode* child = node->children[idx];
            if (cnt[idx] > 0 && child != nullptr && allowed(checks, prev, idx, horiz)) {
                cnt[idx]--;
                char ch = 'A' + idx;
                string nextWord = ch + word;
                extendLeft(child, prev, nextWord);
                cnt[idx]++;
            }
        }
    }

    void atAnchor(Position pos) {
        anchor = pos;
        for (int dir = 0; dir < 2; dir++) {
            horiz = dir == 0;
            if (horiz) {
                dr = 0;
                dc = 1;
            } else {
                dr = 1;
                dc = 0;
            }
            for (int idx = 0; idx < 26; idx++) {
                const GaddagNode* child = root->children[idx];
                if (cnt[idx] > 0 && child != nullptr && allowed(checks, pos, idx, horiz)) {
                    cnt[idx]--;
                    string word;
                    word += char('A' + idx);
                    extendLeft(child, pos, word);
                    cnt[idx]++;
                }
            }
        }
    }

public:
    GaddagSearch(const Board& b, const string& rack, const GaddagNode* node)
        : board(b), root(node) {
        countRack(rack, cnt);
        makeChecks(board, nullptr, root, checks);
    }

    vector<Move> run() {
        for (int row = 0; row < Board::Size; row++) {
            for (int col = 0; col < Board::Size; col++) {
                if (checks.cell[row][col].anchor) {
                    Position pos{row, col};
                    atAnchor(pos);
                }
            }
        }
        return rankMoves(board, moves);
    }

    vector<Move> runAt(Position pos) {
        if (board.withinBoard(pos) && checks.cell[pos.row][pos.column].anchor) {
            atAnchor(pos);
        }
        return rankMoves(board, moves);
    }
};
}

vector<Move> generateMovesGADDAG(const Board& board, const string& rack, ScrabbleGaddag& dict) {
    if (!validRack(rack)) {
        vector<Move> empty;
        return empty;
    }
    GaddagSearch search(board, rack, dict.getRoot());
    return search.run();
}

vector<Move> generateMovesAtSquare(const Board& board, const string& rack,ScrabbleGaddag& dict, Position pos) {
    if (!validRack(rack)) {
        vector<Move> empty;
        return empty;
    }
    GaddagSearch search(board, rack, dict.getRoot());
    return search.runAt(pos);
}
