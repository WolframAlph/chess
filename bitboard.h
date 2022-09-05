#ifndef CCHESS_BITBOARD_H
#define CCHESS_BITBOARD_H

typedef unsigned long long Bitboard;

#define set_bit(bitboard, n) ((bitboard) |= (1ULL << (n)))
#define clear_bit(bitboard, n) ((bitboard) &= ~(1ULL << (n)))
#define get_lsb(bitboard) ({de_bruijn_index[(((bitboard) & -(bitboard)) * 0x03f79d71b4cb0a89) >> 58];})
#define pop_lsb(bitboard) ({byte _idx = get_lsb((bitboard)); (bitboard) &= ((bitboard)-1); _idx;})
#define pop_cnt(bitboard) ({byte _cnt = 0; for(Bitboard _cpy = (bitboard); _cpy; _cpy &= _cpy - 1) _cnt++; _cnt;})

#endif //CCHESS_BITBOARD_H
