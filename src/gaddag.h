#include <string>
#include <vector>

#ifndef DSA_PROJECT_2_SCRABBLE_GADDAG_H
#define DSA_PROJECT_2_SCRABBLE_GADDAG_H
class GaddagNode {
    public:
        GaddagNode* children[27]; 
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
        bool includes(std::string word); 
        std::string printWords(); 
        int getSize(); 
    private:
        std::vector<std::string> printWordsHelper(GaddagNode* curr, int letterIndex);
        int getSizeHelper(GaddagNode* curr);
};

void createDictionary(std::string file, ScrabbleGaddag& dictionary);

#endif 