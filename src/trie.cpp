//
// Created by mcgra on 10/28/2026.
//
#include <sstream>
#include <filesystem>
#include <iostream>
#include <map>
#include "trie.h"


TrieNode::TrieNode() {
    endofWord = false;
    for (int i = 0; i < 26; i++) {
        childrens[i] = nullptr;
    }
}

TrieNode::~TrieNode() {
    for (int i = 0; i < 26; i++) {
        if (childrens[i]) {
            delete childrens[i];
        }
    }
}


ScrabbleTrie::ScrabbleTrie() {
    root = new TrieNode();
}
ScrabbleTrie::~ScrabbleTrie() {
    delete root;
}

void ScrabbleTrie::insert(std::string word) {
    TrieNode* curr = root;
    for (char c : word) {
        int index = c - 'a';
        if (!curr->childrens[index]) {
            curr->childrens[index] = new TrieNode();
        }
        curr=curr->childrens[index];
    }
    curr->endofWord = true;
}

bool ScrabbleTrie::search(std::string word) {
    TrieNode* curr = root;
    for (char c : word) {
        int index = c - 'a';
        if (!curr->childrens[index]) {
            std::cout<<"not found!"<<std::endl;
            return false;
        }
        curr=curr->childrens[index];
    }
    return curr->endofWord;
}






void createDictionary(std::string filepath,ScrabbleTrie& dictionary) {
    std::ifstream dict(filepath);
    if (dict.is_open()) {
        std::string word;

        while (std::getline(dict, word)) {
            dictionary.insert(word);
        }
        dict.close();
        std::cout<<"success!"<<std::endl;
    }else {
        std::cout<<"error opening file";
    }

}

void ScrabbleTrie::deleteWord(std::string word) {
    TrieNode* curr=root;
    for (char c : word) {
        int index = c - 'a';
        if (!curr->childrens[index]) {
            return;
        }
        curr=curr->childrens[index];
    }
    if (curr->endofWord==true) {
        curr->endofWord = false;
        std::cout<<"deleted"<<std::endl;
    }

}
std::map<char,int> createScores() {
    std::map<char, int> scores = {
        {'a', 1},{'b', 3},{'c', 3},{'d', 2},{'e', 1},{'f', 4},{'g', 2},
{'h', 4},{'i', 1},{'j', 8},{'k', 5},{'l', 1},{'m', 3},{'n', 1},{'o', 1},{'p', 3},{'q', 10},{'r', 1},{'s', 1},
{'t',1},{'u', 1},{'v', 4},{'w',4},{'x', 8},{'y', 4},{'z', 10}
    };

    return scores;

};

int sumScores(ScrabbleTrie &dictionary, std::string word) {
    if (!dictionary.search(word)) {
        return 0;
    }
    std::map<char, int> scores=createScores();
    int score=0;
    for (char c : word) {
        auto it=scores.find(c);
        score+=it->second;
    }
    return score;
};
