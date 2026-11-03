#include "board.h"
using namespace std;
#include <iomanip>
#include <stdexcept>



Board::Board() {
    for (int row = 0; row < Size; row++) {
        for (int col = 0; col < Size; col++) {
            grid[row][col].letter = ' ';
        }
    }
}

bool Board::containLetters(const string& txt) const {
    for (char letter : txt) {
        if ( letter < 'A' || letter > 'Z') {
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

bool Board::placeWord(const string& word, Position start, Position end,
                      string& rack, string& msg) {
    msg.clear();
    if (word.empty() || word.size() > Size || !containLetters(word)) {
        msg = "word must contain 1-15 letters (A-Z).";
        return false;
    }
    if (rack.empty() || rack.size() > 7 || !containLetters(rack)) {
        msg = "rack has to contain 1-7 letters (A-Z). ";
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
        msg = "The ending position must be to the right of or below the starting position. Cannot in the left direction. ";
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
    } else if (end.column > start.column) {
        dc = 1;
    }

    string rackLeft = rack;
    int added = 0;
    Position pos = start;

    
    for (int idx = 0; idx < wordLen; idx++) {
        Square sq = grid[pos.row][pos.column];
        char letter = word[idx];

        if (sq.isEmpty()) {
            size_t rackIdx = rackLeft.find(letter);
            if (rackIdx == string::npos) {
                msg = string("Your rack does not have enough ") + letter
                    + " tiles for this placement.";
                return false;
            }
            rackLeft.erase(rackIdx, 1);
            added++;
        } else if (sq.letter != letter) {
            string coord;
            char colChar = 'A' + pos.column;
            coord += colChar;
            coord += to_string(pos.row + 1);
            msg = coord + " already contains " + sq.letter
                + ", but this word requires " + letter + ".";
            return false;
        }

        pos.row += dr;
        pos.column += dc;
    }

    if (added == 0) {
        msg = "Place at least one new tile; that word is already on the board.";
        return false;
    }

    // Everything passed. Now place the word and update the real rack.
    pos = start;
    for (int idx = 0; idx < wordLen; idx++) {
        grid[pos.row][pos.column].letter = word[idx];
        pos.row += dr;
        pos.column += dc;
    }
    rack = rackLeft;
    return true;
}

void Board::print(ostream& out) const {
    printCol(out);
    printBorder(out);

    for (int row = 0; row < Size; row++) {
        out << setw(2) << row+ 1 << " |";
        for (int col = 0; col < Size; col++) {
            char ch = grid[row][col].letter;
            if (grid[row][col].isEmpty() && row == Size / 2 && col == Size / 2) {
                ch = '*';
            }
            out << ' ' << ch << " |";
        }
        out << ' ' << setw(2) << row + 1 << '\n';
        printBorder(out);
    }

    printCol(out);
    out << "\n  * = Starting square (H8)\n";
}
