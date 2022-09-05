#include "board.h"
#include "stdio.h"
#include "movegen.h"
#include "uci.h"


void init()
{
    init_sliders_attacks();
    init_board();
}

int main() {
//    init();
//    parse_fen("r1bqkb1r/1ppp1ppp/2n2n2/p3P3/2Bp4/5N2/PPP2PPP/RNBQK2R w KQkq a6");
//    parse_fen("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10 ");
//    perft_test(1);

    init();
    uci();

    // TODO: killer moves - done, principal variation search, history moves - done, late move reduction
    // TODO: evaluate along the way - DONE
    // TODO : write tests... perft, zobrist, implement hash table for zobrist testing
    // TODO: try extending TT with buckets instead of overwriting
    // TODO: rate killer moves
}
