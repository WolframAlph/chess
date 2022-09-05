#include "board.h"
#include "movegen.h"
#include "stdio.h"

static unsigned long long perft(int depth) {
    if (!depth) return 1;

    unsigned long long nodes = 0;
    MoveList lst = new_move_list();
    Board cpy = BOARD_COPY();

    generate_pseudo_legal_moves(&lst);

    for (int i = 0; i < lst.count; i++)
    {
        if (!make_move(lst.moves[i]))
        {
            BOARD_SET(&cpy);
            continue;
        }
        nodes += perft(depth - 1) ;
        BOARD_SET(&cpy);
    }

    return nodes;
}

void perft_test(int depth)
{
    unsigned long long total = 0;

    MoveList lst = new_move_list();
    Board cpy = BOARD_COPY();

    generate_pseudo_legal_moves(&lst);

    for (int i = 0; i < lst.count; i++)
    {
        if (!make_move(lst.moves[i]))
        {
            BOARD_SET(&cpy);
            continue;
        }
        unsigned long nodes = perft(depth - 1) ;
        BOARD_SET(&cpy);
        print_move(lst.moves[i]);
        printf(": %lu\n", nodes);
        total += nodes;
    }

    printf("\nNodes searched: %llu \n\n", total);
}