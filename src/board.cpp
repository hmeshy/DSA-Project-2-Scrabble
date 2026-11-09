#include "board.h"
#include "move.h"
using namespace std;
#include <iomanip>
#include <stdexcept>
#include <fstream>
#include <cctype>



Board::Board() {
    for (int row = 0; row < Size; row++) {
        for (int col = 0; col < Size; col++) {
            grid[row][col].letter = ' ';
        }
    }

    Position tw[] = {{0,0}, {0,7}, {0,14}, {7,0}, {7,14}, {14,0}, {14,7}, {14,14}};
    Position dw[] = {
        {1,1}, {1,13}, {2,2}, {2,12}, {3,3}, {3,11}, {4,4}, {4,10}, {7,7},
        {10,4}, {10,10}, {11,3}, {11,11}, {12,2}, {12,12}, {13,1}, {13,13}
    };
    Position tl[] = {
        {1,5}, {1,9}, {5,1}, {5,5}, {5,9}, {5,13},
        {9,1}, {9,5}, {9,9}, {9,13}, {13,5}, {13,9}
    };
    Position dl[] = {
        {0,3}, {0,11}, {2,6}, {2,8}, {3,0}, {3,7}, {3,14},
        {6,2}, {6,6}, {6,8}, {6,12}, {7,3}, {7,11},
        {8,2}, {8,6}, {8,8}, {8,12}, {11,0}, {11,7}, {11,14},
        {12,6}, {12,8}, {14,3}, {14,11}
    };

    for (Position pos : tw) {
        grid[pos.row][pos.column].wordMult = 3;
    }
    for (Position pos : dw) {
        grid[pos.row][pos.column].wordMult = 2;
    }
    for (Position pos : tl) {
        grid[pos.row][pos.column].letterMult = 3;
    }
    for (Position pos : dl) {
        grid[pos.row][pos.column].letterMult = 2;
    }
}

bool Board::containLetters(const string& txt) const {
    for (char letter : txt) {
        if ((letter < 'A' || letter > 'Z') && (letter < 'a' || letter > 'z')) {
            return false;
        }
    }
    return true;
}

bool Board::withinBoard(Position pos) const {
    if (pos.row < 0 ||  pos.row >= Size) {
        return false;
    }
    if (pos.column < 0 || pos.column >= Size ) {
        return false;
    }
    return true;
}

void Board::printCol(ostream& out) const {
    out << "    ";
    for (int col = 0; col < Size; col++) {
        char colChar = 'A' + col;
        out << ' ' << colChar << "  ";
    }
    out << '\n';
}

void Board::printBorder(ostream& out) const {
    out << "   +";
    for (int col = 0; col < Size; col++) {
        out << "---+";
    }
    out << '\n';
}

const Square& Board::getSquare(int row, int col) const {
    if (row < 0 || row >= Size || col < 0 || col >= Size) {
        throw out_of_range("Board position must be between 0 and 14." );
    }
    return grid[row][col];
}

bool Board::isEmpty() const {
    for (int row = 0; row < Size; row++) {
        for (int col = 0; col < Size; col++) {
            if (!grid[row][col].isEmpty()) {
                return false;
            }
        }
    }
    return true;
}

bool Board::loadFile(const string& path, string& msg) {
    msg.clear();
    ifstream file(path);
    if (!file.is_open()) {
        msg = "Cannot open board file: " + path;
        return false;
    }

    Board temp;
    string line;
    for (int row = 0; row < Size; row++) {
        if (!getline(file, line)) {
            msg = "The board file must contain exactly 15 rows.";
            return false;
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.length() != Size) {
            msg = "Row " + to_string(row + 1) + " must contain exactly 15 characters.";
            return false;
        }
        for (int col = 0; col < Size; col++) {
            char ch = line[col];
            if (ch == '.') {
                temp.grid[row][col].letter = ' ';
            } else if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
                temp.grid[row][col].letter = ch;
            } else {
                msg = "Use A-Z for normal tiles, a-z for blanks, and dots for empty squares (row "
                    + to_string(row + 1) + ").";
                return false;
            }
        }
    }
    if (getline(file, line)) {
        msg = "The board file must contain exactly 15 rows.";
        return false;
    }
    if (file.bad()) {
        msg = "There was a problem reading the board file.";
        return false;
    }
    *this = temp;
    return true;
}

bool Board::isAnchor(Position pos) const {
    if (!withinBoard(pos)) {
        return false;
    }
    if (isEmpty()) {
        return pos.row == 7 && pos.column == 7;
    }
    return (grid[pos.row][pos.column].isEmpty()) &&  hasNeighbor(pos);
}

bool Board::isOccupied(Position pos) const {
    return withinBoard(pos) && !grid[pos.row][pos.column].isEmpty();
}
char Board::getLetter(Position pos) const {
    if  (!withinBoard(pos) ) {
        return ' ';
    }
    return grid[pos.row][pos.column].letter;
} 

bool Board::hasNeighbor(Position pos) const {
    if (pos.row > 0 && !grid[pos.row - 1][pos.column].isEmpty()) {
        return true;
    }
    if (pos.row < Size- 1 && !grid[pos.row + 1][pos.column].isEmpty()) {
        return true;
    }
    if (pos.column > 0 && !grid[pos.row][pos.column- 1].isEmpty()) {
        return true;
    }
    if (pos.column < Size - 1 && !grid[pos.row][pos.column+ 1].isEmpty()) {
        return true;
    }
    return false;
}

string Board::read(Position pos, int dr, int dc) const {
    if (!withinBoard(pos) || grid[pos.row][pos.column].isEmpty()) {
        return "";
    }

    
    Position prev = pos;
    prev.row -= dr;
    prev.column -= dc;
    while (withinBoard(prev) && !grid[prev.row][prev.column].isEmpty()) {
        pos = prev;
        prev.row -= dr;
        prev.column -= dc;
    }

    string word;
    while (withinBoard(pos) && !grid[pos.row][pos.column].isEmpty()) {
        word += grid[pos.row][pos.column].letter;
        pos.row += dr;
        pos.column += dc;
    }
    return word;
}

string Board::getWord(Position start, Position end) const {
    if (!withinBoard(start) || !withinBoard(end)) {
        return "";
    }

    string full;
    if (start.row == end.row) {
        full = read(start, 0, 1);
    } else {
        full = read(start, 1, 0);
    }

    
    if (full.length() == 1) {
        full = read(start, 1, 0);
    }
    return full;
}

bool Board::inDict(const string& word, ScrabbleTrie& dict) const {
    return hasTrieWord(dict.getRoot(), word);
}

bool Board::placeWord(const string& word,Position start, Position end,string& rack, ScrabbleTrie& dict, string& msg) {
    msg.clear();
    if (word.empty() || word.size() > Size || !containLetters(word)) {
        msg = "Word must contain 1-15 letters (A-Z).";
        return false;
    }
    if (!validRack(rack)) {
        msg = "Your rack must contain 1-7 letters.";
        return false;
    }
    if (!withinBoard(start) || !withinBoard(end) ) {
        msg = "Both positions must be on the board (A1 through O15).";
        return false;
    }
    if (start.row != end.row  && start.column != end.column) {
        msg = "Positions must be in the same row or column and diagonal placement is not allowed.";
        return false;
    }
    if (end.row < start.row || end.column < start.column ) {
        msg = "The ending position must be to the right of or below the starting position. Cannot in the left direction.";
        return false;
    }

    int length;
    if (start.row == end.row) {
        length = end.column - start.column +1;
    } else {
        length = end.row - start.row+ 1;
    }

    int wordLen = static_cast<int>(word.length());
    if (wordLen != length) {
        msg = "Those positions cover "+ to_string(length)
            + " squares, but the word has " + to_string(wordLen) + " letters.";
        return false;
    }

    int dr = 0;
    int dc = 0;
    if (end.row > start.row) {
        dr = 1;
    } else {
        dc = 1;
    }

    string rackLeft = rack;
    for (char& ch : rackLeft) {
        ch = static_cast<char>(toupper(static_cast<unsigned char>(ch)));
    }
    string tiles = word;

    // Handle blanks first
    Position pos = start;
    for (char ch : word) {
        if (!isOccupied(pos) && ch >= 'a' && ch <= 'z') {
            size_t idx = rackLeft.find('?');
            if (idx == string::npos) {
                msg = "Your rack does not have enough blanks to play this move.";
                return false;
            }
            rackLeft.erase(idx, 1);
        }
        pos.row += dr;
        pos.column += dc;
    }
    int added = 0;
    bool touch = false;
    bool center = false;
    pos = start;

    // Handle standard letters
    for (int idx = 0; idx < wordLen; idx++) {
        Square sq = grid[pos.row][pos.column];
        char letter = word[idx];

        if (sq.isEmpty()) {
            if (letter >= 'A' && letter <= 'Z') {
                size_t rackIdx = rackLeft.find(letter);
                if (rackIdx == string::npos) {
                        msg = string("Your rack does not have enough ") + letter
                            + " tiles for this placement.";
                        return false;
                }
                rackLeft.erase(rackIdx, 1);
            }
            added++;
        } else if (toupper(static_cast<unsigned char>(sq.letter))
                   != toupper(static_cast<unsigned char>(letter))) {
            string coord;
            char colChar = 'A' +pos.column;
            coord += colChar;
            coord += to_string(pos.row + 1);
            msg = coord + " already contains " + sq.letter
                + ", but this word requires " + letter + ".";
            return false;
        }

        if (!sq.isEmpty()|| hasNeighbor(pos)) {
            touch = true;
        }
        if (pos.row == 7 && pos.column == 7) {
            center = true;
        }

        pos.row += dr;
        pos.column += dc;
    }

    if (added == 0) {
        msg = "Place at least one new tile; that word is already on the board.";
        return false;
    }
    bool first = isEmpty();
    if (first && !center) {
        msg =  "The first word must cover the center star at H8.";
        return false;
    }


    if (first && wordLen < 2) {
        msg =  "The first word must contain at least two letters.";
        return false;
    }
    if (!first && !touch) {
        msg = "The word must connect to the tiles already on the board.";
        return false;
    }
    Board temp = *this;
    pos = start;
    for (int idx = 0; idx < wordLen; idx++) {
        if (grid[pos.row][pos.column].isEmpty()) {
            temp.grid[pos.row][pos.column].letter = tiles[idx];
        }
        pos.row += dr;
        pos.column += dc;
    }
    string full = temp.getWord(start, end);
    if (full.length() < 2 || !inDict(full, dict)) {
        msg = "The word " + full + " is not in the dictionary.";
        return false;
    }
    pos = start;
    for (int idx = 0; idx < wordLen; idx++) {
        if (grid[pos.row][pos.column].isEmpty()) {
            string cross = temp.read(pos, dc, dr);
            if (cross.length() > 1 && !inDict(cross, dict)) {
                msg = "The crossing word "+ cross + " is not in the dictionary.";
                return false;
            }
        }
        pos.row += dr;
        pos.column += dc;
    }

    *this = temp;
    rack = rackLeft;
    return true;
}

void Board::print(ostream& out) const {
    printCol(out);
    printBorder(out);

    for (int row = 0; row < Size; row++) {
        out << setw(2) << row+ 1 << " |";
        for (int col = 0; col < Size; col++) {
            const Square& sq = grid[row][col];
            string label = " ";
            if (!sq.isEmpty()) {
                label = sq.letter;
            } else if (row == Size / 2 && col == Size / 2) {
                label = "*";
            } else if (sq.wordMult == 3) {
                label = "TW";
            } else if (sq.wordMult == 2) {
                label = "DW";
            } else if (sq.letterMult == 3) {
                label = "TL";
            } else if (sq.letterMult == 2) {
                label = "DL";
            }
            out << ' ' << left << setw(2) << label << right << '|';
        }
        out << ' ' << setw(2) << row + 1 << '\n';
        printBorder(out);
    }

    printCol(out);
    out << "\n  DL = Double letter | TL = Triple letter\n";
    out << "  DW = Double word   | TW = Triple word\n";
    out << "  * = Starting square (H8), also double word\n";
    out << "  Lowercase letter = blank tile (0 points)\n";
}
