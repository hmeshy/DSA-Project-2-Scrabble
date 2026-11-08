#ifndef DSA_PROJECT_2_SCRABBLE_BOARD_H
#define DSA_PROJECT_2_SCRABBLE_BOARD_H

#include <ostream>
#include <string>
#include "trie.h"
using namespace std;
struct Square {
    // Uppercase = normal tile, lowercase = assigned blank, space = empty.
    char letter;
    int letterMult = 1;
    int wordMult = 1;

    bool isEmpty() const {
        return letter == ' ';
    }
};

struct Position {
    int row;
    int column;
};

class Board {
public:
    static const int Size = 15;

    Board();

    const Square& getSquare(int row, int col) const;
    void print(ostream& out) const;
    bool isEmpty() const;
    bool loadFile(const string& path, string& msg);
    bool isAnchor(Position pos) const;
    bool isOccupied(Position pos) const;
    char getLetter(Position pos) const;
    string getWord(Position start, Position end) const;
    bool placeWord(const string& word, Position start, Position end,
                   string& rack, ScrabbleTrie& dict, string& msg);
    bool withinBoard(Position pos) const;
private:
    Square grid[Size][Size];

    bool containLetters(const string& txt) const;
    bool hasNeighbor(Position pos) const;
    string read(Position pos, int dr, int dc) const;
    bool inDict(const string& word, ScrabbleTrie& dict) const;
    void printCol(ostream& out) const;
    void printBorder(ostream& out) const;
};

#endif
