
#include <fstream>
#include <iostream>
#include "gaddag.h"

namespace {
bool cleanWord(std::string& word) {
    if (word.empty()) {
        return false;
    }
    for (char& ch : word) {
        if (ch >= 'A' && ch <= 'Z') {
            ch = ch - 'A' + 'a';
        }
        if (ch < 'a' || ch > 'z') {
            return false;
        }
    }
    return true;
}
}

GaddagNode::GaddagNode() {
    terminal = false;
    for (int i = 0; i < 27; i++) {
        children[i] = nullptr;
    }
}

GaddagNode::~GaddagNode() {
    for (int i = 0; i < 27; i++) {
        if (children[i]) {
            delete children[i];
        }
    }
}


ScrabbleGaddag::ScrabbleGaddag() {
    root = new GaddagNode();
}

ScrabbleGaddag::~ScrabbleGaddag() {
    delete root;
}

GaddagNode* ScrabbleGaddag::getRoot() {
    return root;
}

void ScrabbleGaddag::insert(std::string word) {
    if (!cleanWord(word)) return;
    int len = static_cast<int>(word.length());
    for (int i = 0; i < len; i++) {
        int firstInd = i;
        int currInd = firstInd;
        GaddagNode* curr = root;
        while (currInd >= 0) {
            char currChar = word[currInd];
            int index = currChar - 'a';
            if (!curr->children[index]) {
                curr->children[index] = new GaddagNode();
            }
            curr=curr->children[index];
            currInd--;
        }
        currInd = firstInd + 1;
        if (currInd != len) {
            if (!curr->children[26]) {
                curr->children[26] = new GaddagNode();
            }
            curr=curr->children[26];
        }
        while (currInd < len) {
            char currChar = word[currInd];
            int index = currChar - 'a';
            if (!curr->children[index]) {
                curr->children[index] = new GaddagNode();
            }
            curr=curr->children[index];
            currInd++;
        }
        curr->terminal = true;
    }
}

bool ScrabbleGaddag::includes(std::string word) {
    if (!cleanWord(word)) {
        return false;
    }
    int len = static_cast<int>(word.length());
    for (int i = 0; i < len; i++) {
        int firstInd = i;
        int currInd = firstInd;
        GaddagNode* curr = root;
        while (currInd >= 0) {
            char currChar = word[currInd];
            int index = currChar - 'a';
            if (!curr->children[index]) {
                return false;
            }
            curr=curr->children[index];
            currInd--;
        }
        currInd = firstInd + 1;
        if (currInd != len) {
            if (!curr->children[26]) {
                return false;
            }
            curr=curr->children[26];
        }
        while (currInd < len) {
            char currChar = word[currInd];
            int index = currChar - 'a';
            if (!curr->children[index]) {
                return false;
            }
            curr=curr->children[index];
            currInd++;
        }
        if (!curr->terminal) {
            return false;
        }
    }
    return true;
}

std::string ScrabbleGaddag::printWords() {
    std::string words;
    for (int i = 0; i < 27; i++) {
        if (root->children[i]) {
            std::vector<std::string> letterWords = printWordsHelper(root->children[i], i);
            for (std::string word : letterWords) {
                words += (word + "\n");
            }
        }
    }
    return words;
}

std::vector<std::string> ScrabbleGaddag::printWordsHelper(GaddagNode* curr, int letterIndex) {
    char letter = 'A' + letterIndex;
    if (letterIndex == 26) {
        letter = '*';
    }
    std::vector<std::string> results;
    if (curr->terminal) {
        results.push_back(std::string{letter});
    }
    for (int i = 0; i < 27; i++) {
        if (curr->children[i]) {
            std::vector<std::string> letterWords = printWordsHelper(curr->children[i], i);
            for (std::string word : letterWords) {
                results.push_back(letter + word);
            }
        }
    }
    return results;
}
int ScrabbleGaddag::getSize() {
    int size = 0;
    for (int i = 0; i < 27; i++) {
        if (root->children[i]) {
            int letterSize = getSizeHelper(root->children[i]);
            size += letterSize;
        }
    }
    return size;
}
int ScrabbleGaddag::getSizeHelper(GaddagNode* curr) {
    int size = 0;
    if (curr->terminal) {
        size++;
    }
    for (int i = 0; i < 27; i++) {
        if (curr->children[i]) {
            int letterSize = getSizeHelper(curr->children[i]);
            size += letterSize;
        }
    }
    return size;
}


void createDictionary(std::string filepath,ScrabbleGaddag& dictionary) {
    std::ifstream dict(filepath);
    if (dict.is_open()) {
        std::string word;

        while (std::getline(dict, word)) {
            dictionary.insert(word);
        }
        dict.close();
        std::cout<<"success!"<<std::endl;
    }   else {
        std::cout<<"error opening file";
    }
}
