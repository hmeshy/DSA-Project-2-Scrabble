#include <iostream>
#include "trie.h"

using namespace std;

int main() {

    ScrabbleTrie bruh;
    createDictionary("../assets/enable1.txt", bruh);
    int score=sumScores(bruh,"hello");
    int bad=sumScores(bruh,"bjkgfdd");
    cout<<"score: "<<score<<endl;
    cout<<"no score: "<<bad<<endl;
}