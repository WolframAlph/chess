# chess

compile
`gcc *.c -o shitchess`

run
`./shitchess`

### commands

`uci` - display uci info. [UCI explanation](https://en.wikipedia.org/wiki/Universal_Chess_Interface)

`ucinewgame` - set board to initial state.

`d` - display ascii board and current fen representation. [FEN explanation](https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation)


`position fen <fen string>` - set board state to provided fen string. (e.g. `position fen rnq1k2r/pp1b1ppp/4pn2/8/2BP4/4PN2/P3QPPP/R1B1K2R b KQkq - 0 5`)

`go depth <depth>` - search for best move in current position to specified `<depth>`. (e.g. `go depth 7`).

`go perft <depth>` - run perft test on current position to specified `<depth>`(see number of possible positions after `<depth>` moves). (e.g. `go perft depth 6`).

### play engine
1. Download any UCI compatible chess GUI.
2. Compile engine and configure GUI to use engine.
3. Play with fixed search depth. Try 7 and increase if it does not take long for engine to respond.
