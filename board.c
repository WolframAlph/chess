#include "string.h"
#include "stdio.h"
#include "board.h"
#include "transposition.h"
#include "eval.h"

Board board;

void clear_board()
{
    memset(&board, 0, sizeof(Board));
    memset(board.square_to_piece, no_piece, sizeof(board.square_to_piece));
    board.en_passant = no_sq;
}

void init_board()
{
    clear_board();
    init_tt();
    clear_history();
    clear_killers();
    board.position_hash = calculate_position_hash();
}

void parse_fen(char* fen) {
    byte square = a8;
    byte piece;

    while (*fen != ' ')
    {
        if (*fen >= '0' && *fen <= '9')
        {
            square += *fen - '0';
        }
        else
        {
            piece = fen_char_to_piece[*fen];
            board.square_to_piece[square] = piece;
            set_bit(board.pieces[piece], square++);
        }

        fen++;

        if (*fen == '/')
        {
            square -= 16;
            fen++;
        }
    }

    fen++;

    if (*fen == 'w') board.turn = white;
    else board.turn = black;

    fen += 2;

    if (*fen != '-')
    {
        while (*fen != ' ')
        {
            switch (*fen++) {
                case 'K': board.castle |= wk; break;
                case 'Q': board.castle |= wq; break;
                case 'k': board.castle |= bk; break;
                case 'q': board.castle |= bq; break;
            }
        }
        fen++;
    } else {
        fen += 2;
    }

    if (*fen != '-')
    {
        byte file = *fen++ - 'a';
        byte rank = *fen - '1';
        board.en_passant = rank * 8 + file;
    }

    for (int i = K; i <= P; i++)
    {
        board.occupancy[white] |= board.pieces[i];
        board.occupancy[black] |= board.pieces[i+6];
    }


    for (byte piece = K; piece <= P; piece++)
    {
        Bitboard piece_bitboard = board.pieces[piece];
        while(piece_bitboard)
        {
            board.material[white] += pval(piece);
            board.positional_score[white] += pos_val(piece, pop_lsb(piece_bitboard));
        }
    }

    for (byte piece = k; piece <= p; piece++)
    {
        Bitboard piece_bitboard = board.pieces[piece];
        while(piece_bitboard)
        {
            board.material[white] -= pval(piece);
            board.positional_score[white] -= pos_val(piece, pop_lsb(piece_bitboard));
        }
    }
}


void get_current_fen(char *fen_buff)
{
    byte empty = 0;
    byte piece;

    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; file++)
        {
            piece = board.square_to_piece[rank*8+file];
            if (piece != no_piece)
            {
                if (empty)
                {
                    *fen_buff++ = (char)('0' + empty);
                    empty = 0;
                }

                *fen_buff++ = ascii_pieces[piece];
            }
            else empty++;
        }

        if (empty) *fen_buff++ = (char)('0' + empty);
        if (rank) *fen_buff++ = '/';
        empty = 0;
    }

    *fen_buff++ = ' ';

    if (board.turn == white) *fen_buff++ = 'w';
    else *fen_buff++ = 'b';

    *fen_buff++ = ' ';

    if (!board.castle)
        *fen_buff++ = '-';
    else
    {
        if (board.castle & wk)
            *fen_buff++ = 'K';
        if (board.castle & wq)
            *fen_buff++ = 'Q';
        if (board.castle & bk)
            *fen_buff++ = 'k';
        if (board.castle & bq)
            *fen_buff++ = 'q';
    }

    *fen_buff++ = ' ';

    if (board.en_passant == no_sq)
        *fen_buff++ = '-';
    else
    {
        memcpy(fen_buff, squares_ascii[board.en_passant], 2);
        fen_buff += 2;
    }

    *fen_buff = '\0';
}

void print_fen()
{
    char fen[100];
    get_current_fen(fen);
    printf("%s\n", fen);
}

void print_ascii_board() {
    byte piece;
    for (int rank = 7; rank >= 0; rank--)
    {
        printf("+---+---+---+---+---+---+---+---+\n");
        printf("| ");
        for (int file = 0; file < 8; file++)
        {
            piece = board.square_to_piece[rank*8+file];
            if (piece != no_piece) printf("%c", ascii_pieces[piece]);
            else printf(" ");
            printf(" | ");
        }
        printf("%d", rank + 1);
        printf("\n");
    }
    printf("+---+---+---+---+---+---+---+---+\n  a   b   c   d   e   f   g   h\n\n");
}

void print_bitboard(Bitboard bitboard) {
    int offset = 56;
    int rankMask = 0b11111111;
    int rank;
    for (; offset >= 0; offset -= 8) {
        printf("+---+---+---+---+---+---+---+---+\n");
        printf("| ");
        rank = (int) (bitboard >> offset) & rankMask;
        for (int j = 0; j < 8; j++) {
            if ((rank >> j) & 1) printf("X");
            else printf(" ");
            printf(" | ");
        }
        printf("\n");
    };
    printf("+---+---+---+---+---+---+---+---+\n");
    printf("  a   b   c   d   e   f   g   h\n");
    printf("\n");
}
