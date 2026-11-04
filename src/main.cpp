#include <iostream>
#include "trie.h"
#include "gaddag.h"

using namespace std;

int main() {

    ScrabbleTrie bruh;
    createDictionary("../assets/enable1.txt", bruh);
    int score=sumScores(bruh,"hello");
    int bad=sumScores(bruh,"bjkgfdd");
    cout<<"score: "<<score<<endl;
    cout<<"no score: "<<bad<<endl;

    ScrabbleGaddag bigGaddag;
    createDictionary("../assets/enable1.txt", bigGaddag);
    cout << "Created GADDAG with " << bigGaddag.getSize() << " word representations." << endl;
    cout << "includes good: " << bigGaddag.includes("good") << endl;
    cout << "includes abacaba: " << bigGaddag.includes("abacaba") << endl;
    ScrabbleGaddag smallGaddag;
    smallGaddag.insert("hello");
    smallGaddag.insert("bye");
    smallGaddag.insert("by");
    smallGaddag.insert("hell");
    std::cout << smallGaddag.printWords();
    
}