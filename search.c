#include <sys/time.h>
#include "search.h"
#include "movegen.h"
#include "board.h"
#include "eval.h"
#include "stdio.h"

#define INFINITY 9999999
#define ASP_WINDOW 50


static int quiescence(int alpha, int beta, PvLine *prev, int ply, int *nodes)
{
    int max_eval = evaluate();

    ++(*nodes);

    if (max_eval >= beta)
        return max_eval;

    if (max_eval > alpha)
        alpha = max_eval;

    Board cpy = BOARD_COPY();
    MoveList lst = new_move_list();

    generate_pseudo_legal_moves(&lst);
    score_moves(&lst, ply, prev);

    for (int i = 0; i < lst.count; i++)
    {
        pick_move(&lst, i);

        if (!is_capture(lst.moves[i]))
            continue;

        if (!make_move(lst.moves[i]))
        {
            BOARD_SET(&cpy);
            continue;
        }

        int score = -quiescence(-beta, -alpha, prev, ply+1, nodes);

        BOARD_SET(&cpy);

        if (score > max_eval)
            max_eval = score;

        if (score > alpha)
            alpha = score;

        if (alpha >= beta)
            return max_eval;
    }

    return max_eval;
}

static int negamax(int alpha, int beta, int depth, int ply, int null, PvLine *curr, PvLine *prev, int *nodes) {
    int score;

    if ((score = read_tt_entry(board.position_hash, alpha, beta, depth)) != NO_ENTRY)
        return score;

    if (depth == 0 || ply == MAX_SEARCH_DEPTH)
        return quiescence(alpha, beta, prev, ply+1, nodes);

    ++(*nodes);

    byte in_check = is_in_check(board.turn == white ? K: k);
    Board cpy = BOARD_COPY();

    if (ply && depth >= 3 && null && !in_check)
    {
        board.turn ^= 1;
        board.position_hash ^= turn_key;

        if (board.en_passant != no_sq)
        {
            board.position_hash ^= en_passant_keys[board.en_passant];
            board.en_passant = no_sq;
        }

        PvLine line;
        line.move_count = 0;

        score = -negamax(-beta, -beta+1, depth - 1 - 2, ply+1, 0, &line, prev, nodes);

        BOARD_SET(&cpy);

        if (score >= beta)
            return score;
    }

    int legal_moves = 0;
    tt_flag flag = HASH_ALPHA;

    MoveList lst = new_move_list();

    generate_pseudo_legal_moves(&lst);
    score_moves(&lst, ply, prev);

    for (int i = 0; i < lst.count; i++)
    {
        pick_move(&lst, i);

        if (!make_move(lst.moves[i]))
        {
            BOARD_SET(&cpy);
            continue;
        }

        legal_moves++;

        PvLine line;
        line.move_count = 0;

        score = -negamax(-beta, -alpha, depth - 1, ply + 1, 1, &line, prev, nodes);

        BOARD_SET(&cpy);

        if (score > alpha)
        {
            alpha = score;
            flag = HASH_EXACT;

            if (alpha >= beta)
            {
                if (!is_capture(lst.moves[i]))
                {
                    write_history(board.position_hash, lst.moves[i]);
                    write_killer(ply, 1, read_killer(ply, 0));
                    write_killer(ply, 0, lst.moves[i]);
                }

                write_tt_entry(board.position_hash, alpha, depth, HASH_BETA);
                return alpha;
            }

            curr->moves[0] = lst.moves[i];
            memcpy(curr->moves + 1, line.moves, line.move_count * sizeof(move));
            curr->move_count = 1 + line.move_count;

        }
    }

    if (!legal_moves)
    {
        if (in_check)
            return -MATE;
        return 0;
    }

    write_tt_entry(board.position_hash, alpha, depth, flag);
    return alpha;
}

int get_time_ms()
{
    struct timeval time_value;
    gettimeofday(&time_value, NULL);
    return time_value.tv_sec * 1000 + time_value.tv_usec / 1000;
}

move search(int depth)
{
    PvLine prev;
    int alpha = -INFINITY;
    int beta = INFINITY;
    int start = get_time_ms();

    int research = 0;

    for (int current_depth = 1; current_depth <= depth; current_depth++)
    {
        PvLine curr;
        int nodes = 0;

        int score = negamax(alpha, beta, current_depth, 0, 1, &curr, &prev, &nodes);

        if (score <= alpha || score >= beta)
        {
            alpha = -INFINITY;
            beta = INFINITY;
            current_depth--;
            research++;
            continue;
        }

        alpha = score - ASP_WINDOW;
        beta = score + ASP_WINDOW;

        printf("info score cp %d depth %d nodes %d time %d pv ", score, current_depth, nodes, get_time_ms() - start);
        for (int i = 0; i < curr.move_count; i++)
        {
            print_move(curr.moves[i]);
            printf(" ");
        }

        printf("\n");
        fflush(stdout);
        prev = curr;
    }

    printf("Re searched: %d\n", research);
    fflush(stdout);
    return prev.moves[0];
}