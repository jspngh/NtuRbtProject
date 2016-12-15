#ifndef MINIMAX_H
#define MINIMAX_H

#include <vector>

#include "board.hpp"

using namespace std;

/**
 * Returns the best move (as a column number)
 *
 * Parameters:
 *      player:  0 -- Red
 *               1 -- Yellow
 */
int nextMove(int player, Board board, int depth);


/**
 * Simple heuristic to evaluate board configurations
 * Heuristic is (num of 4-in-a-rows)*99999 + (num of 3-in-a-rows)*100 +
 * (num of 2-in-a-rows)*10 - (num of opponent 4-in-a-rows)*99999 - (num of opponent
 * 3-in-a-rows)*100 - (num of opponent 2-in-a-rows)*10
 */
int evaluateBoard(int player, Board board);

#endif

int nextMove(int player, Board board, int depth)
{
    vector<int> legal_moves;
    for (int col = 0; col < BOARD_WIDTH; col++)
    {
        if (board.isMoveLegal(col))
        {
            legal_moves.push_back(col);
        }
    }

    if (depth == 0 || legal_moves.size() == 0)
        return evaluateBoard(player, board);

    if (player == 0) /*Maximizing player*/
    {
        int alpha = -999999;
        auto vector<int>::iterator it = legal_moves.begin();
        while (it != legal_moves.end())
        {

            it++;
        }
    }
    else /*Minimizing player */
    {
    }
}
