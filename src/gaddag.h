// TODO - implement all of the Big 5 or use smart pointers
// TODO - consider a more consise representation of the Gaddag later on

#include <string>
#include <vector>

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
        GaddagNode* getRoot();
        bool includes(std::string word); // check if all representations of a word are included
        std::string printWords(); // use BFS to print (ONLY USE FOR SMALL CASES!)
        int getSize(); // returns number of words; good test for iteration, correctness
    private:
        std::vector<std::string> printWordsHelper(GaddagNode* curr, int letterIndex);
        int getSizeHelper(GaddagNode* curr);
};

void createDictionary(std::string file, ScrabbleGaddag& dictionary);
int expectedWords(std::string file);

#endif //DSA_PROJECT_2_SCRABBLE_TRIE_H