#include <iostream>
#include "trie.h"

using namespace std;

int main() {

    ScrabbleTrie bruh;
    createDictionary("../assets/enable1.txt", bruh);
    bruh.search("aardvark");
    bruh.search("eifsdjfklsad");
    bruh.deleteWord("aardvark");
    if (bruh.search("aardvark")) {
        cout<<"found aardvark"<<endl;
    };
}