#ifndef CCHESS_CONST_H
#define CCHESS_CONST_H

#include "bitboard.h"

typedef unsigned char byte;
typedef unsigned int move;

enum square {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8, no_sq
};

enum piece_typ {
    king,
    queen,
    rook,
    bishop,
    knight,
    pawn
};

enum piece {
    K, Q, R, B, N, P,
    k, q, r, b, n, p,
    no_piece
};

enum side { white, black };

enum castle_rights {
    wk = 1 << 0,
    wq = 1 << 1,
    bk = 1 << 2,
    bq = 1 << 3
};

extern const char *squares_ascii[64];
extern const byte fen_char_to_piece[];
extern const byte ascii_pieces[12];
extern const byte de_bruijn_index[64];
extern const byte rook_relevant_bits[64];
extern const byte bishop_relevant_bits[64];
extern const unsigned long long rook_magic_numbers[64];
extern const unsigned long long bishop_magic_numbers[64];
extern const Bitboard pawn_attacks[2][64];
extern const Bitboard king_attacks[64];
extern const Bitboard knight_attacks[64];
extern const Bitboard king_side_castle_mask[2];
extern const Bitboard queen_side_castle_mask[2];
extern const byte castling_map[64];

#endif //CCHESS_CONST_H
