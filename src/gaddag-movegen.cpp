#include "gaddag-movegen.h"
#include <algorithm>
#include <cctype>

namespace {
class GaddagSearch {
    const Board& board;
    const GaddagNode* root;
    MoveChecks checks{};
    int cnt[26];
    int blanks;
    int dr;
    int dc;
    bool horiz;
    Position anchor;
    vector<Move> moves;

    void saveMove(const string& word, Position start) {
        if (word.length() < 2) {
            return;
        }
        int len = static_cast<int>(word.length());
        Position prev{start.row - dr, start.column - dc};
        Position next{start.row + dr * len, start.column + dc * len};
        if (board.isOccupied(prev) || board.isOccupied(next)) {
            return;
        }
        Move move;
        move.word = word;
        move.startRow = start.row;
        move.startCol = start.column;
        move.horizontal = horiz;
        moves.push_back(move);
    }

    void extendRight(const GaddagNode* node, Position pos, const string& word, Position start) {
        if (node->terminal && !board.isOccupied(pos)) {
            saveMove(word, start);
        }
        if (!board.withinBoard(pos)) {
            return;
        }
        Position next{pos.row + dr, pos.column + dc};
        if (board.isOccupied(pos)) {
            char ch = board.getLetter(pos);
            int idx = toupper(static_cast<unsigned char>(ch)) - 'A';
            if (node->children[idx] != nullptr) {
                extendRight(node->children[idx], next, word + ch, start);
            }
            return;
        }
        for (int idx = 0; idx < 26; idx++) {
            const GaddagNode* child = node->children[idx];
            if (child == nullptr || !allowed(checks, pos, idx, horiz)) {
                continue;
            }
            if (cnt[idx] > 0) {
                cnt[idx]--;
                extendRight(child, next, word + char('A' + idx), start);
                cnt[idx]++;
            }
            if (blanks > 0) {
                blanks--;
                extendRight(child, next, word + char('a' + idx), start);
                blanks++;
            }
        }
    }

    void extendLeft(const GaddagNode* node, Position pos, const string& word, Position start) {
        if (!board.isOccupied(pos)) {
            if (node->terminal) {
                saveMove(word, start);
            }
            // Child 26 switches from the reversed prefix to the forward suffix.
            if (node->children[26] != nullptr) {
                Position next{anchor.row + dr, anchor.column + dc};
                extendRight(node->children[26], next, word, start);
            }
        }
        if (!board.withinBoard(pos)) {
            return;
        }
        Position prev{pos.row - dr, pos.column - dc};
        if (board.isOccupied(pos)) {
            char ch = board.getLetter(pos);
            int idx = toupper(static_cast<unsigned char>(ch)) - 'A';
            if (node->children[idx] != nullptr) {
                extendLeft(node->children[idx], prev, ch + word, pos);
            }
            return;
        }
        // An earlier anchor generates moves using this square itself.
        if (checks.cell[pos.row][pos.column].anchor) {
            return;
        }
        for (int idx = 0; idx < 26; idx++) {
            const GaddagNode* child = node->children[idx];
            if (child == nullptr || !allowed(checks, pos, idx, horiz)) {
                continue;
            }
            if (cnt[idx] > 0) {
                cnt[idx]--;
                extendLeft(child, prev, char('A' + idx) + word, pos);
                cnt[idx]++;
            }
            if (blanks > 0) {
                blanks--;
                extendLeft(child, prev, char('a' + idx) + word, pos);
                blanks++;
            }
        }
    }

    void atSquare(Position pos) {
        anchor = pos;
        for (int dir = 0; dir < 2; dir++) {
            horiz = dir == 0;
            dr = 1;
            dc = 0;
            if (horiz) {
                dr = 0;
                dc = 1;
            }
            Position prev{pos.row - dr, pos.column - dc};
            for (int idx = 0; idx < 26; idx++) {
                const GaddagNode* child = root->children[idx];
                if (child == nullptr || !allowed(checks, pos, idx, horiz)) {
                    continue;
                }
                if (cnt[idx] > 0) {
                    cnt[idx]--;
                    extendLeft(child, prev, string(1, char('A' + idx)), pos);
                    cnt[idx]++;
                }
                if (blanks > 0) {
                    blanks--;
                    extendLeft(child, prev, string(1, char('a' + idx)), pos);
                    blanks++;
                }
            }
        }
    }

public:
    GaddagSearch(const Board& b, const string& rack, const GaddagNode* node)
        : board(b), root(node) {
        countRack(rack, cnt);
        blanks = static_cast<int>(count(rack.begin(), rack.end(), '?'));
        makeChecks(board, nullptr, root, checks);
    }

    vector<Move> run() {
        for (int row = 0; row < Board::Size; row++) {
            for (int col = 0; col < Board::Size; col++) {
                if (checks.cell[row][col].anchor) {
                    atSquare(Position{row, col});
                }
            }
        }
        return rankMoves(board, moves);
    }

    vector<Move> runAt(Position pos) {
        if (board.withinBoard(pos) && checks.cell[pos.row][pos.column].anchor) {
            atSquare(pos);
        }
        return rankMoves(board, moves);
    }
};
}

vector<Move> generateMovesGADDAG(const Board& board, const string& rack, ScrabbleGaddag& dict) {
    if (!validRack(rack)) {
        return {};
    }
    GaddagSearch search(board, rack, dict.getRoot());
    return search.run();
}

vector<Move> generateMovesAtSquare(const Board& board, const string& rack, ScrabbleGaddag& dict, Position pos) {
    if (!validRack(rack)) {
        return {};
    }
    GaddagSearch search(board, rack, dict.getRoot());
    return search.runAt(pos);
}
