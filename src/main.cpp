#include <iostream>
#include <fstream>
#include "board.h"
#include "cli.h"
#include "trie.h"
using namespace std;

int main() {

    string path = "assets/enable1.txt";
    ifstream file(path);
    if (!file.is_open()) {
        path = "../assets/enable1.txt";
        file.clear();
        file.open(path);
    }
    if (!file.is_open()) {
        cerr << "Cannot find assets/enable1.txt. Run from the project or build folder.\n";
        return 1;
    }
    file.close();

    ScrabbleTrie bruh;
    createDictionary(path, bruh);
    int score=sumScores(bruh,"hello");
    int bad=sumScores(bruh,"bjkgfdd");
    cout<<"score: "<<score<<endl;
    cout<<"no score: "<<bad<<endl;

    Board board;
    std::cout << "Scrabble Board\n\n";
    runWord(board, bruh, std::cin, std::cout);
    return 0;
}
