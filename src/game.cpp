#include "game.h"
#include <algorithm>
#include <random>

TileBag::TileBag() {
    int counts[26] = {9,2,2,4,12,2,3,2,9,1,1,4,2,6,8,2,1,6,4,6,4,2,2,1,2,1};
    for (int idx = 0; idx < 26; idx++) {
        for (int n = 0; n < counts[idx]; n++) {
            tiles += char('A' + idx);
        }
    }
    random_device rd;
    mt19937 rng(rd());
    shuffle(tiles.begin(), tiles.end(), rng);
}

void TileBag::refill(string& rack) {
    while (rack.length() < 7 && !tiles.empty()) {
        rack += tiles.back();
        tiles.pop_back();
    }
}

int TileBag::left() const {
    return static_cast<int>(tiles.length());
}
