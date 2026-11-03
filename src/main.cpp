#include <iostream>
#include "board.h"
#include "cli.h"
#include "trie.h"
using namespace std;

int main() {

    ScrabbleTrie bruh;
    createDictionary("../assets/enable1.txt", bruh);
    int score=sumScores(bruh,"hello");
    int bad=sumScores(bruh,"bjkgfdd");
    cout<<"score: "<<score<<endl;
    cout<<"no score: "<<bad<<endl;

    Board board;
    std::cout << "Scrabble Board\n\n";
    runWordEntry(board, std::cin, std::cout);
    return 0;
}
