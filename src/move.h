#ifndef SCRABBLE_MOVE_H
#define SCRABBLE_MOVE_H

#include "board.h"
#include "gaddag.h"
#include <string>
#include <vector>

struct Move {
    string word;
    int startRow;
    int startCol;
    bool horizontal;
    int score = 0;
};

struct CellCheck {
    bool across[26];
    bool down[26];
    bool anchor;
};

struct MoveChecks {
    CellCheck cell[Board::Size][Board::Size];
};

bool validRack(const string& rack);
void countRack(const string& rack, int cnt[26]);
bool hasTrieWord(const TrieNode* root, const string& word);
bool hasGaddagWord(const GaddagNode* root, const string& word);
void crossParts(const Board& board, Position pos, bool horiz, string& before, string& after);
void makeChecks(const Board& board, const TrieNode* trie, const GaddagNode* gad, MoveChecks& checks);
bool allowed(const MoveChecks& checks, Position pos, int idx, bool horiz);
int scoreWord(const string& word);
int scoreMove(const Board& board, const Move& move);
Move placedMove(const Board& board, Position start, Position end);
Position moveEnd(const Move& move);
string moveKey(const Board& board, const Move& move);
vector<Move> rankMoves(const Board& board, vector<Move> moves);
bool sameMoves(const vector<Move>& a, const vector<Move>& b);
string printMove(const Move& move);
string printMove(const Move& move, int score);

#endif
