// TODO - implement all of the Big 5 or use smart pointers
// TODO - consider a more consise representation of the Gaddag later on

#include <string>
#include <map>
#include <fstream>
#ifndef DSA_PROJECT_2_SCRABBLE_GADDAG_H
#define DSA_PROJECT_2_SCRABBLE_GADDAG_H
class GaddagNode {
    public:
        GaddagNode* children[27]; // the 27th "letter" is the delimeter between reverse and forward
        bool terminal;
        GaddagNode();
        ~GaddagNode();
};


class ScrabbleGaddag{
    GaddagNode* root;
    public:
    ScrabbleGaddag();
    ~ScrabbleGaddag();
    void insert(std::string word);
    bool includes(std::string word);
};

void createDictionary(std::string file, ScrabbleGaddag& dictionary);

std::map<char,int> createScores();

#endif //DSA_PROJECT_2_SCRABBLE_TRIE_H