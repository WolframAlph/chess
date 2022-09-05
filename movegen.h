#ifndef CCHESS_MOVEGEN_H
#define CCHESS_MOVEGEN_H

#include "const.h"
#include "search.h"
#include "search.h"

// flag  promoted   to    from   piece
// 0000    0000   000000  000000  0000

enum move_flag {
    CAPTURE = 1 << 0,
    EN_PASSANT = 1 << 1,
    DOUBLE_ADVANCE = 1 << 2,
    CASTLE = 1 << 3,
};

typedef struct {
    move moves[256];
    int scores[256];
    byte count;
} MoveList;

void init_sliders_attacks();
void print_move(move);
void generate_pseudo_legal_moves(MoveList*);
void score_moves(MoveList*, int, PvLine*);
void pick_move(MoveList*, int);
char make_move(move);
byte is_attacked(byte, byte);

#define encode_move(piece, from, to, promoted, flag) ({(piece) | ((from) << 4) | ((to) << 10) | ((promoted) << 16) | ((flag) << 20);})
#define get_move_piece(move) ((move) & 0x0f)
#define get_move_source_square(move) (((move) >> 4) & 0x3f)
#define get_move_target_square(move) (((move) >> 10) & 0x3f)
#define get_move_promoted_piece(move) (((move) >> 16) & 0x0f)
#define get_move_flag(move) (((move) >> 20) & 0x0f)

#define get_piece_type(piece) ((piece) % 6)

#define is_capture(move) (get_move_flag((move)) & CAPTURE)
#define is_in_check(king) is_attacked(get_lsb(board.pieces[(king)]), board.turn ^ 1)

#define new_move_list() ({MoveList _lst; _lst.count = 0; _lst;})
#define add_move(lst, move) ({(lst)->moves[(lst)->count++] = (move);})

#endif //CCHESS_MOVEGEN_H
