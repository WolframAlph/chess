#include "transposition.h"
#include "board.h"
#include "const.h"
#include "movegen.h"

#define TT_HASH_SIZE 30000000
#define HISTORY_HASH_SIZE 400000

zobrist_key castle_keys[16];
zobrist_key en_passant_keys[64];
zobrist_key pieces_keys[12][64];
zobrist_key turn_key;

typedef struct {
    zobrist_key key;
    int score;
    byte depth;
    byte flag;
} tt_entry;

typedef struct {
    zobrist_key key;
    move mv;
} history_entry;

static tt_entry tt[TT_HASH_SIZE];
static history_entry history[HISTORY_HASH_SIZE];
static unsigned int killers[2][64];
static int history_moves[12][64];

static unsigned int rand_state = 1804289383;

static unsigned int rand_32()
{
    unsigned int number = rand_state;
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    rand_state = number;
    return number;
}

static zobrist_key random_key()
{
    zobrist_key n1, n2, n3, n4;

    n1 = (zobrist_key)(rand_32()) & 0xFFFF;
    n2 = (zobrist_key)(rand_32()) & 0xFFFF;
    n3 = (zobrist_key)(rand_32()) & 0xFFFF;
    n4 = (zobrist_key)(rand_32()) & 0xFFFF;

    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

void init_tt()
{
    turn_key = random_key();

    for (int i = 0; i < 12; i++)
        for (int j = 0; j < 64; j++)
            pieces_keys[i][j] = random_key();

    for (int i = 0; i < 64; i++)
        en_passant_keys[i] = random_key();

    for (int i = 0; i < 16; i++)
        castle_keys[i] = random_key();

    memset(tt, 0, sizeof(tt));
}

zobrist_key calculate_position_hash()
{
    zobrist_key key = 0;

    for (byte piece = K; piece <= p; piece++)
    {
        Bitboard piece_bitboard = board.pieces[piece];
        while (piece_bitboard)
            key ^= pieces_keys[piece][pop_lsb(piece_bitboard)];
    }

    if (board.en_passant != no_sq)
        key ^= en_passant_keys[board.en_passant];

    key ^= castle_keys[board.castle];

    if (board.turn == black)
        key ^= turn_key;

    return key;
}

int read_tt_entry(zobrist_key key, int alpha, int beta, int depth)
{
    tt_entry *entry = tt + (key % TT_HASH_SIZE);

    if (key == entry->key && depth <= entry->depth)
    {
        if (entry->flag == HASH_EXACT)
            return entry->score;
        if (entry->flag == HASH_ALPHA && entry->score <= alpha)
            return alpha;
        if (entry->flag == HASH_BETA && entry->score >= beta)
            return beta;
    }

    return NO_ENTRY;
}

void write_tt_entry(zobrist_key key, int score, int depth, tt_flag flag)
{
    tt_entry *entry = tt + (key % TT_HASH_SIZE);
    entry->key = key;
    entry->score = score;
    entry->depth = depth;
    entry->flag = flag;
}

void write_history(zobrist_key key, move mv)
{
    history_entry *entry = history + (key % HISTORY_HASH_SIZE);
    entry->key = key;
    entry->mv = mv;
}

int read_history(zobrist_key key, move mv)
{
    history_entry *entry = history + (key % HISTORY_HASH_SIZE);
    return entry->key == key && entry->mv == mv;
}

void clear_history(void)
{
    memset(history, 0, sizeof history);
}

void write_killer(int ply, int index, move mv)
{
    killers[index][ply] = mv;
}

move read_killer(int ply, int index)
{
    return killers[index][ply];
}

void clear_killers(void)
{
    memset(killers, 0, sizeof killers);
}

void write_history_move(move mv, int depth)
{
    history_moves[get_move_piece(mv)][get_move_target_square(mv)] += depth;
}

int read_history_move(move mv)
{
    return history_moves[get_move_piece(mv)][get_move_target_square(mv)];
}

void clear_history_moves(void)
{
    memset(history_moves, 0, sizeof history_moves);
}