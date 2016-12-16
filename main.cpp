#include <iostream>

#include "board.hpp"
#include "minimax.hpp"

using namespace std;

int main (int argc, char* args[])
{
    Board b;

    int player_user = 0;
    int player_ai = 1;
    int col;
    int depth = 6;

    int winner = -1;

    while (winner == -1)
    {
        // user move
        cout << "give a column please (1-7): ";
        cin >> col;
        col = col - 1;
        b.doMove(col, player_user);

        // check if user won
        if (b.getWinner() != -1)
        {
            winner = player_user;
            break;
        }

        // AI's move
        col = nextMovePython(player_ai, b, depth);
        cout << "next move: " << col << endl;
        b.doMove(col, player_ai);

        // check if AI won
        if (b.getWinner() != -1)
        {
            winner = player_ai;
            break;
        }

        // print board
        cout << b << endl;
    }

    cout << b << endl;

    cout << "game is finished" << endl;
    cout << "winner: " << winner << endl;
}
