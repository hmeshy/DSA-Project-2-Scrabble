#include "cli.h"
#include "trie.h"
using namespace std;
#include <map>
#include <string>

namespace {
int calcWordScore(const string& word) {
    map<char, int> vals = createScores();
    int pts = 0;
    for (char letter : word) {
        char lower = letter - 'A' + 'a';
        pts += vals[lower];
    }
    return pts;
}

bool isWhitespace(char ch) {
    return ch == ' ' || ch == '\t'
        || ch == '\r' || ch == '\n';
}

string normalizeInput(const string& txt) {
    size_t beg = 0;
    size_t end = txt.length();
    while (beg < end && isWhitespace(txt[beg])) {
        beg++;
    }
    while (end > beg && isWhitespace(txt[end - 1])) {
        end--;
    }
    string res = txt.substr(beg, end - beg);
    for (char& ch : res) {
        if (ch >= 'a' && ch <= 'z') {
            ch = ch - 'a' + 'A';
        }
    }
    return res;
}

bool readPrompt(istream& in, ostream& out,const string& label, string& line) {
    out << label;
    out.flush();
    if (!getline(in, line)) {
        return false;
    }
    line = normalizeInput(line);
    return line != ":QUIT";
}

bool isValid(const string& rack) {
    if (rack.empty() || rack.size() > 7) {
        return false;
    }
    for (char letter : rack) {
        if (letter < 'A' || letter > 'Z') {
            return false;
        }
    }
    return true;
}

bool parsePosition(const string& txt, Position& pos) {
    if (txt.length() != 2 && txt.length() != 3) {
        return false;
    }
    if (txt[0] < 'A' || txt[0] > 'O') {
        return false;
    }
    if (txt[1] < '1' || txt[1] > '9') {
        return false;
    }
    int row = 0;
    for (size_t idx = 1; idx < txt.length(); idx++) {
        if (txt[idx] < '0' || txt[idx] > '9') {
            return false;
        }
        row = row * 10 + txt[idx] - '0';
    }
    if (row > Board::Size) {
        return false;
    }
    
    pos.row = row - 1;
    pos.column = txt[0] - 'A';
    return true;
}
} 

void runWord(Board& board, ScrabbleTrie& dict, istream& in, ostream& out) {
    board.print(out);
    out << "\nenter your rack, then the word and its start/end positions.\n"
           << "Type :quit at any time to exit the game :).\n";

    int turn = 1;
    int player = 1;
    while (true) {
        out << "\nTurn " << turn << " - Player " << player << '\n';
        string rack;
        while (true) {
            if (!readPrompt(in, out, "Your rack (1-7 letters): ", rack)) {
                out << "Bye!\n";
                return;
            }
            if (isValid(rack)) {
                break;
            }
            out << "Enter 1-7 letters (A-Z), without spaces or blank tiles.\n";
        }

        while (true) {
            string word;
            string startStr;
            string endStr;
            if (!readPrompt(in, out, "Word: ", word)) {
                out << "Bye!\n";
                return;
            }
            if (!readPrompt(in, out, "Starting position (ex. G8): ", startStr)) {
                out << "Bye!\n";
                return;
            }
            if (!readPrompt(in, out, "Ending position (ex. I8): ", endStr)) {
                out << "Bye!\n";
                return;
            }

            Position start;
            Position end;
            if (!parsePosition(startStr, start)) {
                out << "The starting position must be A1 through O15 .\n"
                       << "Try again with the same rack: " << rack << ".\n\n";
                continue;
            }
            if (!parsePosition(endStr, end)) {
                out << "The ending position must be A1 through O15 .\n"
                       << "Try again with the same rack: " << rack << ".\n\n";
                continue;
            }

            string msg;
            if (!board.placeWord(word, start, end, rack, dict, msg)) {
                out << msg << "\nTry again with the same rack: " << rack << ".\n\n";
                continue;
            }

            out << "\nPlaced " << word << " from " << startStr << " to " << endStr << ".\n";
            string full = board.getWord(start, end);
            if (full != word) {
                out << "Word made: " << full << ".\n";
            }
            out << "Word score: " << calcWordScore(full) << " points.\n";
            out << "Remaining rack: ";
            if (rack.empty()) {
                out << "(empty)";
            } else {
                out << rack;
            }
            out << ".\n\n";
            board.print(out);
            turn++;
            if (player == 1) {
                player = 2;
            } else {
                player = 1;
            }
            break;
        }
    }
}
