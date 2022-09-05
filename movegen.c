#include "movegen.h"
#include "search.h"
#include "board.h"
#include "stdio.h"
#include "eval.h"

static Bitboard bishop_attack_masks[64];
static Bitboard rook_attack_masks[64];

static Bitboard bishop_attacks[64][512];
static Bitboard rook_attacks[64][4096];

static Bitboard set_occupancy(int index, int bits_in_mask, Bitboard attack_mask)
{
    Bitboard occupancy = 0;

    for (int count = 0; count < bits_in_mask; count++)
    {
        int square = get_lsb(attack_mask);
        clear_bit(attack_mask, square);
        if (index & (1 << count))
            occupancy |= (1ULL << square);
    }

    return occupancy;
}

static Bitboard mask_rook_attacks(int square)
{
    Bitboard attacks = 0;

    int r, f;

    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));

    return attacks;
}

static Bitboard mask_bishop_attacks(int square)
{
    Bitboard attacks = 0;

    int r, f;

    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++) attacks |= (1ULL << (r * 8 + f));
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--) attacks |= (1ULL << (r * 8 + f));
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--) attacks |= (1ULL << (r * 8 + f));

    return attacks;
}

static Bitboard bishop_attacks_on_the_fly(int square, Bitboard block)
{
    Bitboard attacks = 0;

    int r, f;

    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--)
    {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) break;
    }

    return attacks;
}

static Bitboard rook_attacks_on_the_fly(int square, Bitboard block)
{
    Bitboard attacks = 0;

    int r, f;

    int tr = square / 8;
    int tf = square % 8;

    for (r = tr + 1; r <= 7; r++)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }

    for (r = tr - 1; r >= 0; r--)
    {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) break;
    }

    for (f = tf + 1; f <= 7; f++)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }

    for (f = tf - 1; f >= 0; f--)
    {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) break;
    }
    return attacks;
}

void init_sliders_attacks()
{
    for (int square = 0; square < 64; square++)
    {
        bishop_attack_masks[square] = mask_bishop_attacks(square);
        rook_attack_masks[square] = mask_rook_attacks(square);

        Bitboard bishop_attack_mask = bishop_attack_masks[square];
        Bitboard rook_attack_mask = rook_attack_masks[square];

        int rook_relevant_bits_count = pop_cnt(rook_attack_mask);
        int bishop_relevant_bits_count = pop_cnt(bishop_attack_mask);

        unsigned long rook_occupancy_indices = (1 << rook_relevant_bits_count);
        unsigned long bishop_occupancy_indices = (1 << bishop_relevant_bits_count);

        for (int index = 0; index < rook_occupancy_indices; index++)
        {
            Bitboard occupancy = set_occupancy(index, rook_relevant_bits_count, rook_attack_mask);
            unsigned long magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - rook_relevant_bits[square]);
            rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
        }

        for (int index = 0; index < bishop_occupancy_indices; index++)
        {
            Bitboard occupancy = set_occupancy(index, bishop_relevant_bits_count, bishop_attack_mask);
            unsigned long magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits[square]);
            bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
        }
    }
}

static inline Bitboard get_bishop_attacks(byte square, Bitboard occupancy)
{
    occupancy &= bishop_attack_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits[square];
    return bishop_attacks[square][occupancy];
}

static inline Bitboard get_rook_attacks(byte square, Bitboard occupancy)
{
    occupancy &= rook_attack_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square];
    return rook_attacks[square][occupancy];
}

static inline Bitboard get_queen_attacks(byte square, Bitboard occupancy)
{
    return get_bishop_attacks(square, occupancy) | get_rook_attacks(square, occupancy);
}

static inline Bitboard get_king_attacks(byte square, Bitboard _)
{
    return king_attacks[square];
}

static inline Bitboard get_knight_attacks(byte square, Bitboard _)
{
    return knight_attacks[square];
}

static Bitboard (*attack_fn[])(byte, Bitboard) = {
        [K] = get_king_attacks,
        [k] = get_king_attacks,
        [Q] = get_queen_attacks,
        [q] = get_queen_attacks,
        [R] = get_rook_attacks,
        [r] = get_rook_attacks,
        [B] = get_bishop_attacks,
        [b] = get_bishop_attacks,
        [N] = get_knight_attacks,
        [n] = get_knight_attacks
};

void print_move(move mv)
{
    byte src = get_move_source_square(mv);
    byte dst = get_move_target_square(mv);
    byte promoted = get_move_promoted_piece(mv);
    printf("%s%s", squares_ascii[src], squares_ascii[dst]);
    if (promoted)
        printf("%c", ascii_pieces[promoted <= 5 ? promoted + 6 : promoted]);
}

byte is_attacked(byte square, byte color)
{
    Bitboard all_occupancy = board.occupancy[white] | board.occupancy[black];
    if (get_king_attacks(square, all_occupancy) & board.pieces[K+(6*color)]) return 1;
    if (get_queen_attacks(square, all_occupancy) & board.pieces[Q+(6*color)]) return 1;
    if (get_rook_attacks(square, all_occupancy) & board.pieces[R+(6*color)]) return 1;
    if (get_bishop_attacks(square, all_occupancy) & board.pieces[B+(6*color)]) return 1;
    if (get_knight_attacks(square, all_occupancy) & board.pieces[N+(6*color)]) return 1;
    if (pawn_attacks[color ^ 1][square] & board.pieces[P+(6*color)]) return 1;

    return 0;
}

static const int mvv_lva[6][6] = {
        {600, 500, 400, 300, 200, 100},
        {601, 501, 401, 301, 201, 101},
        {602, 502, 402, 302, 202, 102},
        {603, 503, 403, 303, 203, 103},
        {604, 504, 404, 304, 204, 104},
        {605, 505, 405, 305, 205, 105},
};

static inline int score_move(move mv, PvLine *prev, int ply)
{
    if (ply < prev->move_count && prev->moves[ply] == mv)
        return 100000;

    if (!is_capture(mv))
    {
        if (read_killer(ply, 0) == mv)
            return 80;

        if (read_killer(ply, 1) == mv)
            return 70;

//      on average speeds up search
        if (read_history(board.position_hash, mv))
            return 60;

        return 0;
    }

    byte attacker = get_piece_type(get_move_piece(mv));
    byte victim = get_piece_type(board.square_to_piece[get_move_target_square(mv)]);
    return mvv_lva[attacker][victim];
}

void score_moves(MoveList *lst, int ply, PvLine *prev)
{
    for (int i = 0; i < lst->count; i ++)
        lst->scores[i] = score_move(lst->moves[i], prev, ply);
}

void generate_pseudo_legal_moves(MoveList *lst)
{
    byte start, stop, friendly_pawns, pawn_direction, pawn_start_rank, pawn_end_rank,
    can_castle_king_side, can_castle_queen_side, king_side_castle_check_square, queen_side_castle_check_square,
    king_start_square, king_side_castle_target_square, queen_side_castle_target_square;

    Bitboard friendly_occupancy = board.occupancy[board.turn];
    Bitboard enemy_occupancy = board.occupancy[board.turn ^ 1];
    Bitboard all_occupancy = board.occupancy[white] | board.occupancy[black];
    Bitboard en_passant_mask = board.en_passant != no_sq ? 1ULL << board.en_passant : 0;

    if (board.turn == white)
    {
        start = K;
        stop = N;
        friendly_pawns = P;
        pawn_direction = 1;
        pawn_start_rank = 1;
        pawn_end_rank = 7;
        can_castle_king_side = board.castle & wk;
        can_castle_queen_side = board.castle & wq;
        king_start_square = e1;
        king_side_castle_target_square = g1;
        queen_side_castle_target_square = c1;
        king_side_castle_check_square = f1;
        queen_side_castle_check_square = d1;
    }
    else
    {
        start = k;
        stop = n;
        friendly_pawns = p;
        pawn_direction = -1;
        pawn_start_rank = 6;
        pawn_end_rank = 0;
        can_castle_king_side = board.castle & bk;
        can_castle_queen_side = board.castle & bq;
        king_start_square = e8;
        king_side_castle_target_square = g8;
        queen_side_castle_target_square = c8;
        king_side_castle_check_square = f8;
        queen_side_castle_check_square = d8;
    }

    move mv;
    byte source_square, target_square;
    Bitboard piece_bitboard, attacks;

    for (byte piece = start; piece <= stop; piece++)
    {
        piece_bitboard = board.pieces[piece];
        while(piece_bitboard)
        {
            source_square = pop_lsb(piece_bitboard);
            attacks = attack_fn[piece](source_square, all_occupancy) & ~friendly_occupancy;

            while(attacks)
            {
                target_square = pop_lsb(attacks);

                if (board.square_to_piece[target_square] == no_piece)
                    mv = encode_move(piece, source_square, target_square, 0, 0);
                else
                    mv = encode_move(piece, source_square, target_square, 0, CAPTURE);

                add_move(lst, mv);
            }
        }
    }

    Bitboard pawns = board.pieces[friendly_pawns];
    while (pawns)
    {
        source_square = pop_lsb(pawns);
        target_square = source_square + 8 * pawn_direction;
        if (board.square_to_piece[target_square] == no_piece)
        {
            if (target_square / 8 == pawn_end_rank)
                for (byte promoted = start + 1; promoted <= stop; promoted++)
                    add_move(lst, encode_move(friendly_pawns, source_square, target_square, promoted, 0));
            else
                add_move(lst, encode_move(friendly_pawns, source_square, target_square, 0, 0));

            if (source_square / 8 == pawn_start_rank)
            {
                target_square += 8 * pawn_direction;
                if (board.square_to_piece[target_square] == no_piece)
                    add_move(lst, encode_move(friendly_pawns, source_square, target_square, 0, DOUBLE_ADVANCE));
            }
        }

        attacks = pawn_attacks[board.turn][source_square] & (enemy_occupancy | en_passant_mask);
        while (attacks)
        {
            target_square = pop_lsb(attacks);

            if (target_square == board.en_passant)
                add_move(lst, encode_move(friendly_pawns, source_square, target_square, 0, EN_PASSANT | CAPTURE));

            else if (target_square / 8 == pawn_end_rank)
                for (byte promoted = start + 1; promoted <= stop; promoted++)
                    add_move(lst, encode_move(friendly_pawns, source_square, target_square, promoted, CAPTURE));

            else
                add_move(lst, encode_move(friendly_pawns, source_square, target_square, 0, CAPTURE));
        }
    }

    if (!is_attacked(king_start_square, board.turn ^ 1))
    {
        if (
                can_castle_king_side &&
                !(all_occupancy & king_side_castle_mask[board.turn]) &&
                !is_attacked(king_side_castle_check_square, board.turn ^ 1)
                )
        {
            add_move(lst, encode_move(start, king_start_square, king_side_castle_target_square, 0, CASTLE));
        }

        if (
                can_castle_queen_side &&
                !(all_occupancy & queen_side_castle_mask[board.turn]) &&
                !is_attacked(queen_side_castle_check_square, board.turn ^ 1)
                )
        {
            add_move(lst, encode_move(start, king_start_square, queen_side_castle_target_square, 0, CASTLE));
        }
    }
}

char make_move(move mv)
{
    byte piece = get_move_piece(mv);
    byte source_square = get_move_source_square(mv);
    byte target_square = get_move_target_square(mv);
    byte flag = get_move_flag(mv);
    byte promoted_piece = get_move_promoted_piece(mv);

    if (flag & CAPTURE)
    {

        if (flag & EN_PASSANT)
        {
            byte sq = target_square + (board.turn == white ? -8 : 8);
            byte target_piece = board.square_to_piece[sq];

            clear_bit(board.pieces[target_piece], sq);
            clear_bit(board.occupancy[board.turn ^ 1], sq);
            board.square_to_piece[sq] = no_piece;

            board.position_hash ^= pieces_keys[target_piece][sq];

            board.material[board.turn^1] -= pval(target_piece);
            board.positional_score[board.turn^1] -= pos_val(target_piece, sq);
        }
        else
        {
            byte target_piece = board.square_to_piece[target_square];
            clear_bit(board.pieces[target_piece], target_square);
            clear_bit(board.occupancy[board.turn ^ 1], target_square);

            board.position_hash ^= pieces_keys[target_piece][target_square];

            board.material[board.turn^1] -= pval(target_piece);
            board.positional_score[board.turn^1] -= pos_val(target_piece, target_square);
        }
    }

    if (flag & DOUBLE_ADVANCE)
    {
        board.en_passant = source_square + (board.turn == white ? 8: -8);
        board.position_hash ^= en_passant_keys[board.en_passant];
    }
    else
        board.en_passant = no_sq;

    if (board.en_passant != no_sq)
        board.position_hash ^= en_passant_keys[board.en_passant];

    if (flag & CASTLE)
    {
        switch (target_square) {
            case g1:
                clear_bit(board.pieces[R], h1);
                set_bit(board.pieces[R], f1);

                clear_bit(board.occupancy[white], h1);
                set_bit(board.occupancy[white], f1);

                board.square_to_piece[h1] = no_piece;
                board.square_to_piece[f1] = R;

                board.position_hash ^= pieces_keys[R][h1];
                board.position_hash ^= pieces_keys[R][f1];

                board.positional_score[board.turn] -= pos_val(R, h1);
                board.positional_score[board.turn] += pos_val(R, f1);
                break;

            case c1:
                clear_bit(board.pieces[R], a1);
                set_bit(board.pieces[R], d1);

                clear_bit(board.occupancy[white], a1);
                set_bit(board.occupancy[white], d1);

                board.square_to_piece[a1] = no_piece;
                board.square_to_piece[d1] = R;

                board.position_hash ^= pieces_keys[R][a1];
                board.position_hash ^= pieces_keys[R][d1];

                board.positional_score[board.turn] -= pos_val(R, a1);
                board.positional_score[board.turn] += pos_val(R, d1);
                break;

            case g8:
                clear_bit(board.pieces[r], h8);
                set_bit(board.pieces[r], f8);

                clear_bit(board.occupancy[black], h8);
                set_bit(board.occupancy[black], f8);

                board.square_to_piece[h8] = no_piece;
                board.square_to_piece[f8] = r;

                board.position_hash ^= pieces_keys[r][h8];
                board.position_hash ^= pieces_keys[r][f8];

                board.positional_score[board.turn] -= pos_val(r, h8);
                board.positional_score[board.turn] += pos_val(r, f8);
                break;

            case c8:
                clear_bit(board.pieces[r], a8);
                set_bit(board.pieces[r], d8);

                clear_bit(board.occupancy[black], a8);
                set_bit(board.occupancy[black], d8);

                board.square_to_piece[a8] = no_piece;
                board.square_to_piece[d8] = r;

                board.position_hash ^= pieces_keys[r][a8];
                board.position_hash ^= pieces_keys[r][d8];

                board.positional_score[board.turn] -= pos_val(r, a8);
                board.positional_score[board.turn] += pos_val(r, d8);
                break;
        }
    }

    board.square_to_piece[target_square] = piece;
    board.square_to_piece[source_square] = no_piece;

    clear_bit(board.pieces[piece], source_square);
    set_bit(board.pieces[piece], target_square);

    clear_bit(board.occupancy[board.turn], source_square);
    set_bit(board.occupancy[board.turn], target_square);

    board.positional_score[board.turn] -= pos_val(piece, source_square);
    board.positional_score[board.turn] += pos_val(piece, target_square);

    board.position_hash ^= pieces_keys[piece][source_square];
    board.position_hash ^= pieces_keys[piece][target_square];

    board.position_hash ^= castle_keys[board.castle];

    board.castle &= castling_map[source_square];
    board.castle &= castling_map[target_square];

    board.position_hash ^= castle_keys[board.castle];

    if (promoted_piece)
    {
        clear_bit(board.pieces[piece], target_square);
        set_bit(board.pieces[promoted_piece], target_square);
        board.square_to_piece[target_square] = promoted_piece;

        board.position_hash ^= pieces_keys[piece][target_square];
        board.position_hash ^= pieces_keys[promoted_piece][target_square];

        board.positional_score[board.turn] += pos_val(promoted_piece, target_square);
        board.positional_score[board.turn] -= pos_val(piece, target_square);

        board.material[board.turn] += pval(promoted_piece);
        board.material[board.turn] -= pval(piece);
    }

    board.turn ^= 1;
    board.position_hash ^= turn_key;

    byte king_square = get_lsb(board.turn == white ? board.pieces[k] : board.pieces[K]);

    if (is_attacked(king_square, board.turn))
        return 0;

    return 1;
}


void pick_move(MoveList *lst, int curr_iter)
{

    int best_score = 0;
    int best_index = curr_iter;

    for (int i = curr_iter; i < lst->count; i++)
    {
        if (lst->scores[i] > best_score)
        {
            best_score = lst->scores[i];
            best_index = i;
        }
    }

    move tmp_mv = lst->moves[curr_iter];
    lst->moves[curr_iter] = lst->moves[best_index];
    lst->moves[best_index] = tmp_mv;

    int tmp_score = lst->scores[curr_iter];
    lst->scores[curr_iter] = lst->scores[best_index];
    lst->scores[best_index] = tmp_score;
}