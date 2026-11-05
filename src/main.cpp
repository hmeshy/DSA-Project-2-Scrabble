#include <iostream>
#include <fstream>
#include "board.h"
#include "cli.h"
#include "trie.h"
#include "gaddag.h"

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

    ScrabbleGaddag bigGaddag;
    createDictionary("../assets/enable1.txt", bigGaddag);
    cout << "Created GADDAG with " << bigGaddag.getSize() << " word representations." << endl;
    cout << "Expected Words: " << expectedWords("../assets/enable1.txt") << endl;
    cout << "includes good: " << bigGaddag.includes("good") << endl;
    cout << "includes abacaba: " << bigGaddag.includes("abacaba") << endl;
    ScrabbleGaddag smallGaddag;
    smallGaddag.insert("hello");
    smallGaddag.insert("bye");
    smallGaddag.insert("by");
    smallGaddag.insert("hell");
    std::cout << smallGaddag.printWords();
    
    Board board;
    std::cout << "Scrabble Board\n\n";
    runWordGADDAG(board, bruh, bigGaddag, std::cin, std::cout);
    return 0;
}
