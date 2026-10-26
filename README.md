# DSA-Project-2-Scrabble
**Project Title:** Using Tries and GADDAGs to Solve Scrabble

# Features
- Takes in a board as a txt file as input (see example board), and runs the command
```bash
load {board.txt}
```
- User types in command like below to get the n highest scoring moves for a given rack (will use faster structure)
```bash
moves {rack} n
```
- You can also compare the timing of the moves for the two structures
```bash
timing {rack} n
```
- On launch, loads the dictionary (with 172823 words)
