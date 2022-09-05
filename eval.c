#include "const.h"
#include "board.h"
#include "movegen.h"

static const int piece_value[] = {
       [king] = 5000,
       [queen] = 1000,
       [rook] = 550,
       [bishop] = 315,
       [knight] = 300,
       [pawn] = 100,
};

static const byte reverse[64] = {
        56, 57, 58, 59, 60, 61, 62, 63,
        48, 49, 50, 51, 52, 53, 54, 55,
        40, 41, 42, 43, 44, 45, 46, 47,
        32, 33, 34, 35, 36, 37, 38, 39,
        24, 25, 26, 27, 28, 29, 30, 31,
        16, 17, 18, 19, 20, 21, 22, 23,
        8, 9, 10, 11, 12, 13, 14, 15,
        0, 1, 2, 3, 4, 5, 6, 7
};

static const int position_score[6][64] = {
        {
            0, 5, 5, -10, -10, 0, 10, 5,
            -30, -30, -30, -30, -30, -30, -30, -30,
            -50, -50, -50, -50, -50, -50, -50, -50,
            -70, -70, -70, -70, -70, -70, -70, -70,
            -70, -70, -70, -70, -70, -70, -70, -70,
            -70, -70, -70, -70, -70, -70, -70, -70,
            -70, -70, -70, -70, -70, -70, -70, -70,
            -70, -70, -70, -70, -70, -70, -70, -70
        },

        {
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            },

        {
            0, 0, 5, 10, 10, 5, 0, 0,
            0, 0, 5, 10, 10, 5, 0, 0,
            0, 0, 5, 10, 10, 5, 0, 0,
            0, 0, 5, 10, 10, 5, 0, 0,
            0, 0, 5, 10, 10, 5, 0, 0,
            0, 0, 5, 10, 10, 5, 0, 0,
            25, 25, 25, 25, 25, 25, 25, 25,
            0, 0, 5, 10, 10, 5, 0, 0
        },

        {
            0, 0, -10, 0, 0, -10, 0, 0,
            0, 0, 0, 10, 10, 0, 0, 0,
            0, 0, 10, 15, 15, 10, 0, 0,
            0, 10, 15, 20, 20, 15, 10, 0,
            0, 10, 15, 20, 20, 15, 10, 0,
            0, 0, 10, 15, 15, 10, 0, 0,
            0, 0, 0, 10, 10, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        },

        {
            0, -10, 0, 0, 0, 0, -10, 0,
            0, 0, 0, 5, 5, 0, 0, 0,
            0, 0, 10, 10, 10, 10, 0, 0,
            0, 0, 10, 20, 20, 10, 5, 0,
            5, 10, 15, 20, 20, 15, 10, 5,
            5, 10, 10, 20, 20, 10, 10, 5,
            0, 0, 5, 10, 10, 5, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
        },

        {
            0, 0, 0, 0, 0, 0, 0, 0,
            10, 10, 0, -10, -10, 0, 10, 10,
            5, 0, 0, 5, 5, 0, 0, 5,
            0, 0, 10, 20, 20, 10, 0, 0,
            5, 5, 5, 10, 10, 5, 5, 5,
            10, 10, 10, 20, 20, 10, 10, 10,
            20, 20, 20, 30, 30, 20, 20, 20,
            0, 0, 0, 0, 0, 0, 0, 0,
        },
};

int pval(byte piece)
{
    return piece_value[get_piece_type(piece)];
}

int pos_val(byte piece, byte square)
{
    byte type = get_piece_type(piece);
    if (piece > 5)
        square = reverse[square];
    return position_score[type][square];
}

int evaluate()
{
//    int score = (board.material[white] + board.positional_score[white]) + (-board.material[black] - board.positional_score[black]);
    int score = 0;
//    int sscore = 0;

    score += board.material[white];
    score += board.positional_score[white];
    score -= board.material[black];
    score -= board.positional_score[black];

//    int sscore = 0;
//    for (byte piece = K; piece <= P; piece++)
//    {
//        byte type = get_piece_type(piece);
//        Bitboard piece_bitboard = board.pieces[piece];
//        while(piece_bitboard)
//        {
//            sscore += piece_value[type];
////            pop_lsb(piece_bitboard);
//            sscore += position_score[type][pop_lsb(piece_bitboard)];
//        }
//    }
//
//    for (byte piece = k; piece <= p; piece++)
//    {
//        byte type = get_piece_type(piece);
//        Bitboard piece_bitboard = board.pieces[piece];
//        while(piece_bitboard)
//        {
//            sscore -= piece_value[type];
////            pop_lsb(piece_bitboard);
//            sscore -= position_score[type][reverse[pop_lsb(piece_bitboard)]];
//        }
//    }
//    assert(score == sscore);

    return board.turn == white ? score : -score;
}