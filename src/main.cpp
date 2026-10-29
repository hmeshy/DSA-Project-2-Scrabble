#include <iostream>
#include "trie.h"

using namespace std;

int main() {

    ScrabbleTrie bruh;
    createDictionary("../assets/enable1.txt", bruh);
    bruh.search("aardvark");
    bruh.search("eifsdjfklsad");
}