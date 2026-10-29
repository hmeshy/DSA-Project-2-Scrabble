//
// Created by mcgra on 10/28/2026.
//
#include <string>
#include <fstream>
#ifndef DSA_PROJECT_2_SCRABBLE_TRIE_H
#define DSA_PROJECT_2_SCRABBLE_TRIE_H
class TrieNode {
    public:
    TrieNode* childrens[26];
    bool endofWord;
    TrieNode();
    ~TrieNode();
};


class ScrabbleTrie{
    TrieNode* root;
    public:
    ScrabbleTrie();
    void insert(std::string word);
    bool search(std::string word);
    void deleteWord(std::string word);

};

void createDictionary(std::string file, ScrabbleTrie& dictionary);






#endif //DSA_PROJECT_2_SCRABBLE_TRIE_H