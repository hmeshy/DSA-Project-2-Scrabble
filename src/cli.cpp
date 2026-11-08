#include "cli.h"
#include "game.h"
#include "move.h"
#include "trie-movegen.h"
#include "gaddag-movegen.h"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;

namespace {
struct Request {
    string rack;
    string solver;
    int n;
    bool timing;
};

bool isSpace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

string trim(const string& txt) {
    size_t beg = 0;
    size_t end = txt.length();
    while (beg < end && isSpace(txt[beg])) {
        beg++;
    }
    while (end > beg && isSpace(txt[end - 1])) {
        end--;
    }
    return txt.substr(beg, end - beg);
}

string upperText(string txt) {
    for (char& ch : txt) {
        if (ch >= 'a' && ch <= 'z') {
            ch = ch - 'a' + 'A';
        }
    }
    return txt;
}

bool readPrompt(istream& in, ostream& out, const string& label, string& line) {
    out << label;
    out.flush();
    if (!getline(in, line)) {
        return false;
    }
    line = trim(line);
    return upperText(line) != ":QUIT";
}

vector<string> split(const string& line) {
    istringstream in(line);
    vector<string> args;
    string word;
    while (in >> word) {
        args.push_back(word);
    }
    return args;
}

bool parsePosition(const string& txt, Position& pos) {
    if (txt.length() != 2 && txt.length() != 3) {
        return false;
    }
    if (txt[0] < 'A' || txt[0] > 'O' || txt[1] < '1' || txt[1] > '9') {
        return false;
    }
    int row = 0;
    for (size_t idx = 1; idx < txt.length(); idx++) {
        if (txt[idx] < '0' || txt[idx] > '9') {
            return false;
        }
        row = row * 10 + txt[idx] - '0';
    }
    if (row > Board::Size) {
        return false;
    }
    pos = Position{row - 1, txt[0] - 'A'};
    return true;
}

bool parseCount(const string& txt, int& n) {
    if (txt.empty() || txt.length() > 2) {
        return false;
    }
    n = 0;
    for (char ch : txt) {
        if (ch < '0' || ch > '9') {
            return false;
        }
        n = n * 10 + ch - '0';
    }
    return n >= 1 && n <= 20;
}

bool loadBoard(Board& board, string path, string& msg) {
    path = trim(path);
    if (path.length() >= 2 && path.front() == '"' && path.back() == '"') {
        path = path.substr(1, path.length() - 2);
    }
    if (path.empty()) {
        msg = "Enter a board file name.";
        return false;
    }
    ifstream file(path);
    if (file.is_open()) {
        return board.loadFile(path, msg);
    }

    bool absolute = path[0] == '/' ||  path[0] == '\\';
    if (path.length() > 1 && path[1] == ':') {
        absolute = true;
    }
    if (!absolute) {
        string other = "../" + path;
        ifstream test(other);
        if (test.is_open()) {
            return board.loadFile(other, msg);
        }
    }
    return board.loadFile(path, msg);
}

string coord(int row, int col) {
    string pos;
    char colChar = 'A' + col;
    pos += colChar;
    pos += to_string(row + 1);
    return pos;
}

void showMoves(const vector<Move>& moves, int n, const string& solver, ostream& out) {
    if (moves.empty()) {
        out << "No legal moves found for that rack.\n";
        return;
    }
    int count = static_cast<int>(moves.size());
    if (n < count) {
        count = n;
    }
    out << solver << " found " << moves.size() << " legal moves. Showing " << count << ".\n";
    out << "Rank  Word             Start  End    Score\n";
    for (int idx = 0; idx < count; idx++) {
        const Move& move = moves[idx];
        Position end = moveEnd(move);
        out << right << setw(4) << idx + 1 << "  " << left << setw(17) << move.word
            << setw(7) << coord(move.startRow, move.startCol)
            << setw(7) << coord(end.row, end.column) << right << move.score << '\n';
    }
    out << right;
}

bool readRequest(const vector<string>& args, const string& gameRack,
                 const string& solver, Request& req, string& msg) {
    req.timing = upperText(args[0]) == "TIMING";
    req.solver = solver;
    size_t countIdx;

    
    if (!gameRack.empty() && args.size() >= 2 && parseCount(args[1], req.n)) {
        req.rack = gameRack;
        countIdx = 1;
    } else {
        if (args.size() < 3) {
            msg = "Use moves RACK N [trie|gaddag] or timing RACK N. N must be 1-20.";
            return false;
        }
        req.rack = upperText(args[1]);
        countIdx = 2;
    }
    if (!validRack(req.rack) || countIdx >= args.size() || !parseCount(args[countIdx], req.n)) {
        msg = "Use 1-7 rack tiles (A-Z or ? for blanks) and a result count from 1 to 20.";
        return false;
    }
    if (!gameRack.empty()) {
        string a = req.rack;
        string b = gameRack;
        sort(a.begin(), a.end());
        sort(b.begin(), b.end());
        if (a != b) {
            msg = "Use your current rack, or type moves N / timing N.";
            return false;
        }
    }
    if (args.size() == countIdx + 2 && !req.timing) {
        req.solver = upperText(args.back());
    } else if (args.size() != countIdx + 1) {
        msg = "Use moves RACK N [trie|gaddag] or timing RACK N.";
        return false;
    }
    if (req.solver != "TRIE" && req.solver != "GADDAG") {
        msg = "Choose trie or gaddag as the solver.";
        return false;
    }
    return true;
}

void showTiming(const Request& req, const Board& board, ScrabbleTrie& dict,
                ScrabbleGaddag& gad, string& solver, ostream& out) {
    auto beg = chrono::steady_clock::now();
    vector<Move> trie = generateMovesTrie(board, req.rack, dict);
    auto mid = chrono::steady_clock::now();
    vector<Move> gaddag = generateMovesGADDAG(board, req.rack, gad);
    auto end = chrono::steady_clock::now();
    double trieMs = chrono::duration<double, milli>(mid - beg).count();
    double gadMs = chrono::duration<double, milli>(end - mid).count();
    out << fixed << setprecision(3);
    out << "Trie: " << trie.size() << " moves in " << trieMs << " ms\n";
    out << "GADDAG: " << gaddag.size() << " moves in " << gadMs << " ms\n";
    if (!sameMoves(trie, gaddag)) {
        out << "Solver results differ. These suggestions need to be checked.\n";
        return;
    }
    out << "Both solvers found the same moves and scores.\n";
    if (trieMs < gadMs) {
        solver = "TRIE";
        showMoves(trie, req.n, solver, out);
    } else {
        solver = "GADDAG";
        showMoves(gaddag, req.n, solver, out);
    }
    out << "Next moves commands will use " << solver << " unless a solver is specified.\n";
}

void query(const vector<string>& args, const Board& board, ScrabbleTrie& dict,
           ScrabbleGaddag& gad, const string& gameRack, string& solver, ostream& out) {
    Request req;
    string msg;
    if (!readRequest(args, gameRack, solver, req, msg)) {
        out << msg << '\n';
        return;
    }
    if (req.timing) {
        showTiming(req, board, dict, gad, solver, out);
        return;
    }

    vector<Move> moves;
    if (req.solver == "TRIE") {
        moves = generateMovesTrie(board, req.rack, dict);
    } else {
        moves = generateMovesGADDAG(board, req.rack, gad);
    }
    showMoves(moves, req.n, req.solver, out);
}

bool analysis(ScrabbleTrie& dict, ScrabbleGaddag& gad, istream& in, ostream& out) {
    Board board;
    string line, msg;
    while (true) {
        if (!readPrompt(in, out, "Board file [board.txt] (:back for menu): ", line)) {
            return false;
        }
        if (upperText(line) == ":BACK") {
            return true;
        }
        if (line.empty()) {
            line = "board.txt";
        }
        if (upperText(line).find("LOAD ") == 0) {
            line = trim(line.substr(5));
        }
        if (loadBoard(board, line, msg)) {
            break;
        }
        out << msg << '\n';
    }
    board.print(out);
    out << "Commands: moves RACK N, timing RACK N, load FILE, show, :back, :quit\n";
    string solver = "GADDAG";
    while (readPrompt(in, out, "analysis> ", line)) {
        vector<string> args = split(line);
        if (args.empty()) {
            continue;
        }
        string cmd = upperText(args[0]);
        if (cmd == ":BACK") {
            return true;
        }
        if (cmd == ":QUIT") {
            return false;
        }
        if (cmd == "MOVES" || cmd == "TIMING") {
            query(args, board, dict, gad, "", solver, out);
        } else if (cmd == "LOAD") {
            string path = trim(line.substr(args[0].length()));
            if (loadBoard(board, path, msg)) {
                out << "Board loaded.\n";
                board.print(out);
            } else {
                out << msg << " The current board was kept.\n";
            }
        } else if (cmd == "SHOW") {
            board.print(out);
        } else {
            out << "Use moves RACK N, timing RACK N, load FILE, show, back, or :quit.\n";
        }
    }
    return false;
}

void finishGame(Player p[2], int winner, ostream& out) {
    int left1 = scoreWord(p[0].rack);
    int left2 = scoreWord(p[1].rack);
    p[0].pts -= left1;
    p[1].pts -= left2;
    if (winner == 0) {
        p[0].pts += left2;
    } else if (winner == 1) {
        p[1].pts += left1;
    }
    out << "\nFinal scores: \n";
    out << "Player 1: " << p[0].pts << "\nPlayer 2: " << p[1].pts << '\n';
    if (p[0].pts == p[1].pts) {
        out << "The game is a tie.\n";
    } else if (p[0].pts > p[1].pts) {
        out << "Player 1 wins!\n";
    } else {
        out << "Player 2 wins!\n";
    }
}

bool playGame(ScrabbleTrie& dict, ScrabbleGaddag& gad, istream& in, ostream& out) {
    Board board;
    TileBag bag;
    Player p[2];
    bag.refill(p[0].rack);
    bag.refill(p[1].rack);
    int player = 0;
    int turn = 1;
    int passes = 0;
    string solver = "GADDAG";
    out << "Racks are dealt and refilled automatically. Two consecutive passes end the game.\n";
    out << "? represents a blank tile, worth 0 points but able to be played as any letter. Use uppercase for normal tiles, lowercase to choose a blank.\n";
    out << "Example: CaT uses a blank as A. Missing uppercase letters use blanks automatically.\n";
    out << "Bonus squares count only for new tiles. Playing 7 tiles in one turn adds 50 points.\n";
    while (true) {
        board.print(out);
        out << "\nTurn " << turn << " - Player " << player + 1 << '\n';
        out << "Player 1: " << p[0].pts << " points | Player 2: " << p[1].pts
            << " points | Tiles left: " << bag.left() << '\n';
        out << "Your rack: " << p[player].rack << '\n';
        while (true) {
            string line;
            if (!readPrompt(in, out, "Word (or moves N, timing N, :pass, :back, :quit): ", line)) {
                return false;
            }
            string word = line;
            string cmd = upperText(line);
            if (cmd == ":BACK") {
                return true;
            }
            if (cmd == ":PASS") {
                passes++;
                if (passes == 2) {
                    finishGame(p, -1, out);
                    return true;
                }
                break;
            }
            vector<string> args = split(line);
            if (!args.empty() && (upperText(args[0]) == "MOVES" || upperText(args[0]) == "TIMING")) {
                query(args, board, dict, gad, p[player].rack, solver, out);
                continue;
            }
            if (word.empty()) {
                continue;
            }
            string from, to;
            if (!readPrompt(in, out, "Starting position (e.g. G8): ", from)) {
                return false;
            }
            if (upperText(from) == ":BACK") {
                return true;
            }
            if (!readPrompt(in, out, "Ending position (e.g. I8): ", to)) {
                return false;
            }
            if (upperText(to) == ":BACK") {
                return true;
            }
            Position start, end;
            if (!parsePosition(upperText(from), start) || !parsePosition(upperText(to), end)) {
                out << "Use coordinates A1 through O15. Your rack and turn were kept.\n";
                continue;
            }
            Board prev = board;
            string msg;
            size_t rackLen = p[player].rack.length();
            if (!board.placeWord(word, start, end, p[player].rack, dict, msg)) {
                out << msg << "\nTry again with the same rack: " << p[player].rack << ".\n";
                continue;
            }
            Move move = placedMove(board, start, end);
            int pts = scoreMove(prev, move);
            p[player].pts += pts;
            out << "Placed " << printMove(move) << ". Move score: " << pts << " points.\n";
            if (rackLen - p[player].rack.length() == 7) {
                out << "All 7 tiles played! Your move score includes a 50-point bonus.\n";
            }
            bag.refill(p[player].rack);
            out << "Your rack after refill: " << p[player].rack << '\n';
            passes = 0;
            if (bag.left() == 0 && p[player].rack.empty()) {
                board.print(out);
                finishGame(p, player, out);
                return true;
            }
            break;
        }
        if (player == 0) {
            player = 1;
        } else {
            player = 0;
        }
        turn++;
    }
}
}

void runMenu(ScrabbleTrie& dict, ScrabbleGaddag& gad, istream& in, ostream& out) {
    string choice;
    while (true) {
        out << "\nScrabble\n1. Load a board for analysis\n2. Two-player game\n3. Exit\n";
        if (!readPrompt(in, out, "Choice: ", choice)) {
            return;
        }
        if (choice == "3") {
            return;
        }
        if (choice == "1") {
            if (!analysis(dict, gad, in, out)) {
                return;
            }
        } else if (choice == "2") {
            if (!playGame(dict, gad, in, out)) {
                return;
            }
        } else {
            out << "Choose 1, 2, or 3.\n";
        }
    }
}
