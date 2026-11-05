#include "cli.h"
#include "gaddag.h"
#include "trie.h"
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
    vector<string> paths = {"assets/enable1.txt", "../assets/enable1.txt"};
    if (argc > 0) {
        string exe = argv[0];
        size_t slash = exe.find_last_of("/\\");
        if (slash != string::npos) {
            string folder = exe.substr(0, slash + 1);
            paths.push_back(folder + "../assets/enable1.txt");
        }
    }
    ifstream file;
    for (const string& path : paths) {
        file.clear();
        file.open(path);
        if (file.is_open()) {
            break;
        }
    }
    if (!file.is_open()) {
        cerr << "Cannot find assets/enable1.txt. Run from the project or build folder.\n";
        return 1;
    }

    cout << "Loading dictionary...\n";
    ScrabbleTrie dict;
    ScrabbleGaddag gad;
    string word;
    int count = 0;
    while (getline(file, word)) {
        if (!word.empty() && word.back() == '\r') {
            word.pop_back();
        }
        if (word.empty()) {
            continue;
        }
        for (char& ch : word) {
            if (ch >= 'A' && ch <= 'Z') {
                ch = ch - 'A' + 'a';
            }
            if (ch < 'a' || ch > 'z') {
                cerr << "The dictionary contains an invalid word.\n";
                return 1;
            }
        }
        dict.insert(word);
        gad.insert(word);
        count++;
    }
    if (file.bad() || count == 0) {
        cerr << "The dictionary could not be loaded.\n";
        return 1;
    }
    cout << "Loaded " << count << " words.\n";
    runMenu(dict, gad, cin, cout);
    return 0;
}
