#include "move.h"
#include <algorithm>
#include <map>
#include <set>

namespace {
char upper(char ch) {
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 'A';
    }
    return ch;
}

bool betterMove(const Move& a, const Move& b) {
    if (a.score != b.score) {
        return a.score > b.score;
    }
    if (a.word != b.word) {
        return a.word < b.word;
    }
    if (a.startRow != b.startRow) {
        return a.startRow < b.startRow;
    }
    if (a.startCol != b.startCol) {
        return a.startCol < b.startCol;
    }
    if (a.horizontal != b.horizontal) {
        return a.horizontal;
    }
    return false;
}
}

bool validRack(const string& rack) {
    if (rack.empty() || rack.length() > 7) {
        return false;
    }
    for (char ch : rack) {
        ch = upper(ch);
        if (ch != '?' && (ch < 'A' || ch > 'Z')) {
            return false;
        }
    }
    return true;
}

void countRack(const string& rack, int cnt[26]) {
    for (int idx = 0; idx < 26; idx++) {
        cnt[idx] = 0;
    }
    for (char ch : rack) {
        if (ch == '?') {
            continue;
        }
        int idx = upper(ch) - 'A';
        if (idx >= 0 && idx < 26) {
            cnt[idx]++;
        }
    }
}

bool hasTrieWord(const TrieNode* root, const string& word) {
    if (word.empty()) {
        return false;
    }
    const TrieNode* node = root;
    for (char ch : word) {
        ch = upper(ch);
        if (ch < 'A' || ch > 'Z' || node == nullptr) {
            return false;
        }
        node = node->childrens[ch - 'A'];
    }
    return node != nullptr && node->endofWord;
}

bool hasGaddagWord(const GaddagNode* root, const string& word) {
    if (word.empty()) {
        return false;
    }
    const GaddagNode* node = root;
    // The GADDAG also stores each complete word in reverse, without a delimiter.
    for (int idx = static_cast<int>(word.length()) - 1; idx >= 0; idx--) {
        char ch = upper(word[idx]);
        if (ch < 'A' || ch > 'Z' || node == nullptr) {
            return false;
        }
        node = node->children[ch - 'A'];
    }
    return node != nullptr && node->terminal;
}

void crossParts(const Board& board, Position pos, bool horiz, string& before, string& after) {
    int dr;
    int dc;
    if (horiz) {
        dr = 1;
        dc = 0;
    } else {
        dr = 0;
        dc = 1;
    }
    before.clear();
    after.clear();
    Position cur{pos.row - dr, pos.column - dc};
    while (board.isOccupied(cur)) {
        before = board.getLetter(cur) + before;
        cur.row -= dr;
        cur.column -= dc;
    }
    cur = Position{pos.row + dr, pos.column + dc};
    while (board.isOccupied(cur)) {
        after += board.getLetter(cur);
        cur.row += dr;
        cur.column += dc;
    }
}

void makeChecks(const Board& board, const TrieNode* trie, const GaddagNode* gad, MoveChecks& checks) {
    // Work out which letters make valid crossing words at each empty square.
    for (int row = 0; row < Board::Size; row++) {
        for (int col = 0; col < Board::Size; col++) {
            Position pos{row, col};
            checks.cell[row][col].anchor = board.isAnchor(pos);
            for (int dir = 0; dir < 2; dir++) {
                bool horiz = dir == 0;
                string before, after;
                crossParts(board, pos, horiz, before, after);
                for (int idx = 0; idx < 26; idx++) {
                    bool ok = false;
                    if (!board.isOccupied(pos)) {
                        if (before.empty() && after.empty()) {
                            ok = true;
                        } else {
                            char ch = 'A' + idx;
                            string word = before + ch + after;
                            if (trie != nullptr) {
                                ok = hasTrieWord(trie, word);
                            } else {
                                ok = hasGaddagWord(gad, word);
                            }
                        }
                    }
                    if (horiz) {
                        checks.cell[row][col].across[idx] = ok;
                    } else {
                        checks.cell[row][col].down[idx] = ok;
                    }
                }
            }
        }
    }
}

bool allowed(const MoveChecks& checks, Position pos, int idx, bool horiz) {
    if (horiz) {
        return checks.cell[pos.row][pos.column].across[idx];
    }
    return checks.cell[pos.row][pos.column].down[idx];
}

int scoreWord(const string& word) {
    static const map<char, int> vals = createScores();
    int pts = 0;
    for (char ch : word) {
        // Lowercase letters are assigned blanks. A rack's '?' is also worth zero.
        if (ch == '?' || (ch >= 'a' && ch <= 'z')) {
            continue;
        }
        if (ch < 'A' || ch > 'Z') {
            return 0;
        }
        char lower = ch - 'A' + 'a';
        pts += vals.at(lower);
    }
    return pts;
}

int scoreMove(const Board& board, const Move& move) {
    int pts = 0;
    Position pos{move.startRow, move.startCol};
    for (char ch : move.word) {
        if (board.isOccupied(pos)) {
            // Keep an old blank worth zero even if the entered word is uppercase.
            pts += scoreWord(string(1, board.getLetter(pos)));
        } else {
            pts += scoreWord(string(1, ch));
            string before, after;
            crossParts(board, pos, move.horizontal, before, after);
            if (!before.empty() || !after.empty()) {
                pts += scoreWord(before + ch + after);
            }
        }
        if (move.horizontal) {
            pos.column++;
        } else {
            pos.row++;
        }
    }
    return pts;
}

Position moveEnd(const Move& move) {
    Position end{move.startRow, move.startCol};
    int offset = static_cast<int>(move.word.length()) - 1;
    if (move.horizontal) {
        end.column += offset;
    } else {
        end.row += offset;
    }
    return end;
}

Move placedMove(const Board& board, Position start, Position end) {
    Move move;
    move.startRow = start.row;
    move.startCol = start.column;
    move.horizontal = start.row == end.row;
    if (!board.isOccupied(start)) {
        return move;
    }
    if (start.row == end.row && start.column == end.column) {
        Position left{start.row, start.column - 1};
        Position right{start.row, start.column + 1};
        move.horizontal = board.isOccupied(left) || board.isOccupied(right);
    }
    int dr;
    int dc;
    if (move.horizontal) {
        dr = 0;
        dc = 1;
    } else {
        dr = 1;
        dc = 0;
    }
    Position prev{start.row - dr, start.column - dc};
    while (board.isOccupied(prev)) {
        start = prev;
        prev.row -= dr;
        prev.column -= dc;
    }
    move.startRow = start.row;
    move.startCol = start.column;
    while (board.isOccupied(start)) {
        move.word += board.getLetter(start);
        start.row += dr;
        start.column += dc;
    }
    return move;
}

string moveKey(const Board& board, const Move& move) {
    string key;
    Position pos{move.startRow, move.startCol};
    for (char ch : move.word) {
        if (!board.isOccupied(pos)) {
            key += to_string(pos.row);
            key += ",";
            key += to_string(pos.column);
            key += ch;
            key += ";";
        }
        if (move.horizontal) {
            pos.column++;
        } else {
            pos.row++;
        }
    }
    return key;
}

vector<Move> rankMoves(const Board& board, vector<Move> moves) {
    for (Move& move : moves) {
        move.score = scoreMove(board, move);
    }
    sort(moves.begin(), moves.end(), betterMove);
    set<string> seen;
    vector<Move> result;
    for (const Move& move : moves) {
        string key = moveKey(board, move);
        if (!key.empty() && seen.count(key) == 0) {
            seen.insert(key);
            result.push_back(move);
        }
    }
    return result;
}

bool sameMoves(const vector<Move>& a, const vector<Move>& b) {
    if (a.size() != b.size()) {
        return false;
    }
    for (size_t idx = 0; idx < a.size(); idx++) {
        const Move& m1 = a[idx];
        const Move& m2 = b[idx];
        if (m1.word != m2.word) {
            return false;
        }
        if (m1.score != m2.score) {
            return false;
        }
        if (m1.startRow != m2.startRow || m1.startCol != m2.startCol) {
            return false;
        }
        if (m1.horizontal != m2.horizontal) {
            return false;
        }
    }
    return true;
}

string printMove(const Move& move) {
    Position end = moveEnd(move);
    string start;
    start += char('A' + move.startCol);
    start += to_string(move.startRow + 1);
    string finish;
    finish += char('A' + end.column);
    finish += to_string(end.row + 1);
    return move.word + " " + start + "-" + finish;
}

string printMove(const Move& move, int score) {
    return to_string(score) + ": " + printMove(move);
}
