#include "trie-movegen.h"
#include <algorithm>
#include <cctype>

namespace {
class TrieSearch {
    const Board& board;
    const TrieNode* root;
    MoveChecks checks{};
    int cnt[26];
    int blanks;
    vector<Move> moves;
    bool horiz;
    int dr;
    int dc;

    bool canStart(Position pos, int rackLen) const {
        Position prev{pos.row - dr, pos.column - dc};
        if (board.isOccupied(prev)) return false;

        int empty = 0;
        while (board.withinBoard(pos)) {
            if (!board.isOccupied(pos)) {
                empty++;
                if (empty > rackLen) return false;
                if (checks.cell[pos.row][pos.column].anchor) return true;
            }
            pos.row += dr;
            pos.column += dc;
        }
        return false;
    }

    void walk(const TrieNode* node, Position pos, Position start,
              const string& word, int added, bool hit) {
        if (node->endofWord && word.length() >= 2 && added > 0 && hit
            && !board.isOccupied(pos)) {
            moves.push_back(Move{word, start.row, start.column, horiz});
        }
        if (!board.withinBoard(pos)) return;

        Position next{pos.row + dr, pos.column + dc};
        if (board.isOccupied(pos)) {
            char ch = board.getLetter(pos);
            int idx = toupper(static_cast<unsigned char>(ch)) - 'A';
            const TrieNode* child = node->childrens[idx];
            if (child != nullptr) walk(child, next, start, word + ch, added, hit);
            return;
        }

        for (int idx = 0; idx < 26; idx++) {
            const TrieNode* child = node->childrens[idx];
            if (child == nullptr || !allowed(checks, pos, idx, horiz)) {
                continue;
            }
            bool touch = hit || checks.cell[pos.row][pos.column].anchor;
            if (cnt[idx] > 0) {
                cnt[idx]--;
                walk(child, next, start, word + char('A' + idx), added + 1, touch);
                cnt[idx]++;
            }
            if (blanks > 0) {
                blanks--;
                walk(child, next, start, word + char('a' + idx), added + 1, touch);
                blanks++;
            }
        }
    }

public:
    TrieSearch(const Board& b, const string& rack, const TrieNode* node)
        : board(b), root(node) {
        countRack(rack, cnt);
        blanks = static_cast<int>(count(rack.begin(), rack.end(), '?'));
        makeChecks(board, root, nullptr, checks);
    }

    vector<Move> run(int rackLen) {
        for (int dir = 0; dir < 2; dir++) {
            horiz = dir == 0;
            dr = 1;
            dc = 0;
            if (horiz) { dr = 0; dc = 1; }
            for (int row = 0; row < Board::Size; row++) {
                for (int col = 0; col < Board::Size; col++) {
                    Position start{row, col};
                    if (canStart(start, rackLen)) walk(root, start, start, "", 0, false);
                }
            }
        }
        return rankMoves(board, moves);
    }
};
}

vector<Move> generateMovesTrie(const Board& board, const string& rack, const ScrabbleTrie& dict) {
    if (!validRack(rack)) return {};
    TrieSearch search(board, rack, dict.getRoot());
    return search.run(static_cast<int>(rack.length()));
}
