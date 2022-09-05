#ifndef CCHESS_TRANSPOSITION_H
#define CCHESS_TRANSPOSITION_H

#include "const.h"

#define NO_ENTRY 1000000

typedef enum { HASH_EXACT, HASH_ALPHA, HASH_BETA } tt_flag;

typedef unsigned long long zobrist_key;


extern zobrist_key castle_keys[16];
extern zobrist_key en_passant_keys[64];
extern zobrist_key pieces_keys[12][64];
extern zobrist_key turn_key;

void init_tt();
zobrist_key calculate_position_hash();
int read_tt_entry(zobrist_key, int, int, int);
void write_tt_entry(zobrist_key, int, int, tt_flag);

void write_history(zobrist_key, move);
int read_history(zobrist_key, move);
void clear_history(void);

void write_killer(int, int, move);
move read_killer(int, int);
void clear_killers(void);

#endif //CCHESS_TRANSPOSITION_H
