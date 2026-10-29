//
// Created by mcgra on 10/28/2026.
//
#include <sstream>
#include <filesystem>
#include <iostream>
#include "trie.h"


TrieNode::TrieNode() {
    endofWord = false;
    for (int i = 0; i < 26; i++) {
        childrens[i] = nullptr;
    }
}

ScrabbleTrie::ScrabbleTrie() {
    root = new TrieNode();
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
