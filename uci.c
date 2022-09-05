#include <string.h>
#include <stdlib.h>
#include "stdio.h"
#include "board.h"
#include "search.h"
#include "movegen.h"
#include "perft.h"


void uci()
{
    char input[500];
    while (1)
    {
        memset(input, 0, sizeof(input));
        fgets(input, 500, stdin);
        char *tok = strtok(strtok(input, "\n"), " ");

        if (strcmp(tok, "position") == 0)
        {
            strtok(NULL, " ");
            char buffer[1024];
            snprintf(buffer, sizeof(buffer), "%s %s %s %s %s", strtok(NULL, " "), strtok(NULL, " "), strtok(NULL, " "), strtok(NULL, " "), strtok(NULL, " "));
            init_board();
            parse_fen(buffer);
        }

        else if (strcmp(tok, "go") == 0)
        {
            tok = strtok(NULL, " ");
            if (strcmp(tok, "depth") == 0)
            {
                move mv = search(atoi(strtok(NULL, " ")));
                printf("bestmove ");
                print_move(mv);
                printf("\n");
            }
            else if (strcmp(tok, "perft") == 0)
            {
                perft_test(atoi(strtok(NULL, " ")));
            }
        }

        else if (strcmp(tok, "isready") == 0)
        {
            printf("readyok\n");
        }

        else if (strcmp(tok, "d") == 0)
        {
            print_ascii_board();
            print_fen();
        }

        else if (strcmp(tok, "quit") == 0)
        {
            exit(0);
        }

        else if (strcmp(tok, "ucinewgame") == 0)
        {
            init_board();
            parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        }

        else if (strcmp(tok, "uci") == 0)
        {
            printf("id name shitchess 1.0\n");
            printf("id author yan\n");
            printf("uciok\n");
        }

        fflush(stdout);
    }
}