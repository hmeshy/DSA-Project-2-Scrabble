# DSA-Project-2-Scrabble
**Project Title:** Using Tries and GADDAGs to Solve Scrabble

# Features
- Takes in a board as a txt file as input (see example board), or starts from a blank board
  
- Move generation and scoring using a GADDAG or a Trie as an underlying data structure

- On launch, loads the dictionary (with 172823 words) as a GADDAG and as a Trie

- Complete scrabble board implementation, with the ability to play a game or analyze a position.

# Scoring
Empty bonus squares are marked `DL` (double letter), `TL` (triple letter),
`DW` (double word), and `TW` (triple word). The center `*` at H8 is also a
double-word square. Placed tiles cover these labels.

Bonus squares apply only when a new tile is placed on them. Letter bonuses are
applied before word bonuses, and multiple word bonuses multiply together.
Each crossing word is scored separately, including the new tile's bonus.
Previously placed tiles count at their normal value; blanks stay worth zero.
A blank can still activate a double-word or triple-word square.

Playing seven new tiles in one turn adds 50 points after all words and
multipliers are scored. A seven-letter word using an old tile does not qualify,
and emptying a rack containing fewer than seven tiles does not qualify.
This scoring is used for both gameplay and the move suggestions from both solvers.

Board files still use letters and dots. The standard bonus layout is added
automatically, and tiles loaded from a file are treated as already played.

# Setup
You can generate the build directory and build the program with the commands
```bash
cmake -B build
cmake --build build
```
and run the program with 
```bash
.\build\DSA_Project_2_Scrabble.exe
```
# Usage
On launch, you are greeted with the following menu:
```
Scrabble
1. Load a board for analysis
2. Two-player game
3. Exit
Choice:
```
Typing "1" prompts you to load the board as a .txt file. An example of an empty board is provided in assets/empty-board.txt. Each square with a tile should be marked with that letter, otherwise with a dot. Regular tiles should be written with an uppercase, while blank tiles should be written with a lowercase - this standard applies on the visualized board later on as well.

After inputting the board, we print a display of the board, alongside a list of possible commands:
```
moves RACK N, timing RACK N, load FILE, show, back, :quit
```

| Command | Purpose |
| -------- | -------- | 
| ```moves RACK N```  | Print highest N scoring moves for a given rack using faster data structure.   |
| ```timing RACK N```  | Time both the GADDAG and Trie implementations on a given rack; print highest N scoring moves for a given rack and set the faster data structure as the default for ```moves```.   |
| ```load FILE```  | Loads the boards at the directory ```FILE```  |
| ```show```  | Prints the board  |
| ```back```  | Returns to the main menu  |
| ```:quit```  | Exits the program  |


The first two commands both print the N highest scoring moves for a given rack, where a rack is a set of up to seven tiles, either letters (i.e. A) or wild cards (denoted ?) that can be played as any letter. If there are less than N legal moves, all legal moves are listed. 

Otherwise, typing "2" prompts you to load a full game where you can play as both players. You can enter a word, starting square, and ending square to play it. Also, you can run the commands ```moves N``` or ```timing N``` for the current rack and board, as well as ```:pass``` to pass your turn and ```:back``` to return to the main menu. Racks are generated randomly from the tile bag, and the game continues until all tiles are played.



