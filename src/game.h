#ifndef SCRABBLE_GAME_H
#define SCRABBLE_GAME_H

#include <string>
using namespace std;

struct Player {
    string rack;
    int pts = 0;
};

class TileBag {
    string tiles;
public:
    TileBag();
    void refill(string& rack);
    int left() const;
};

#endif
