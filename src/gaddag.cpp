
#include <sstream>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <map>
#include "gaddag.h"


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
    // insert all possible bidirectional storage of a word
    // ex: CARE => C * ARE, AC * RE, RAC * E, ERAC
    for (int i = 0; i < word.size(); i++) {
        int firstInd = i;
        int currInd = firstInd;
        GaddagNode* curr = root;
        // add reverse part
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
        // add delimiter
        if (currInd != word.size()) {
            if (!curr->children[26]) {
                curr->children[26] = new GaddagNode();
            }
            curr=curr->children[26];
        }
        // add forward part
        while (currInd < word.size()) {
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
    for (int i = 0; i < word.size(); i++) {
        int firstInd = i;
        int currInd = firstInd;
        GaddagNode* curr = root;
        // add reverse part
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
        // add delimiter
        if (currInd != word.size()) {
            if (!curr->children[26]) {
                return false;
            }
            curr=curr->children[26];
        }
        // add forward part
        while (currInd < word.size()) {
            char currChar = word[currInd];
            int index = currChar - 'a';
            if (!curr->children[index]) {
                return false;
            }
            curr=curr->children[index];
            currInd++;
        }
        // check if terminal
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
int expectedWords(std::string filepath) {
    int count = 0;
    std::ifstream dict(filepath);
    if (dict.is_open()) {
        std::string word;
        while (std::getline(dict, word)) {
            count += word.size();
        }
        dict.close();
    }   else {
        std::cout<<"error opening file";
    }
    return count;
}