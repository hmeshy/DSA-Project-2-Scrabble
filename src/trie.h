//
// Created by mcgra on 10/28/2026.
//

#ifndef DSA_PROJECT_2_SCRABBLE_TRIE_H
#define DSA_PROJECT_2_SCRABBLE_TRIE_H
class TrieNode {
    public:
    TrieNode* childrens[26];
    bool endofWord;
    TrieNode() {
        endofWord = false;
        for (int i = 0; i < 26; i++) {
            childrens[i] = nullptr;
        }
    }
};



class ScrabbleTrie{


};








#endif //DSA_PROJECT_2_SCRABBLE_TRIE_H