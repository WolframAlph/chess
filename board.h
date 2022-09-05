#ifndef CCHESS_BOARD_H
#define CCHESS_BOARD_H

#include "string.h"
#include "const.h"
#include "transposition.h"

typedef struct {
    Bitboard pieces[12];
    Bitboard occupancy[2];
    zobrist_key position_hash;
    int material[2];
    int positional_score[2];
    byte square_to_piece[64];
    byte en_passant;
    byte castle;
    byte turn;
} Board;

extern Board board;

void init_board();
void clear_board();
void parse_fen(char*);
void get_current_fen(char*);
void print_fen();
void print_ascii_board();
void print_bitboard(Bitboard);

#define BOARD_COPY() ({Board _snap; memcpy(&_snap, &board, sizeof(Board)); _snap;})
#define BOARD_SET(current) memcpy(&board, (current), sizeof(Board))

#endif //CCHESS_BOARD_H
