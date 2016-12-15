#include <vector>
#include "minimax.hpp"

using namespace std;

#define DEBUG false
#define INF 99999999
#define FIRST_MOVE -1

struct Node
{
    int score;
    int move;
};

int negamax(int player, Board board, int depth)
{
    if (depth == 0 || board.isFinished()) // TODO: add check to to see if board is completely filled
    {
        return evaluateBoard(player, board);
    }

    int best_score = -INF;
    for (int col = 0; col < BOARD_WIDTH; col++)
    {
        if (board.isMoveLegal(col))
        {
            Board temp = board;
            temp.doMove(col, player);
            int score = - negamax(-player, temp, depth - 1);
            if (score > best_score)
                best_score = score;
        }
    }

    return best_score;
}

int nextMove(int player, Board board, int depth)
{
    vector<Node> legal_moves;
    for (int col = 0; col < BOARD_WIDTH; col++)
    {
        if (board.isMoveLegal(col))
        {
            Board temp = board;
            temp.doMove(col, player);
            int score = - negamax(-player, temp, depth - 1);
            legal_moves.push_back({score, col});
        }
    }

    Node best = {-INF, FIRST_MOVE};
    auto it = legal_moves.begin();
    while (it != legal_moves.end())
    {
        if (it->score > best.score)
        {
            best = *it;
        }
        it++;
    }

    return best.move;
}


// TODO: check correctness, the evaluation is independent of the player
// i.e. this value is always calculated from the point of view of player 1
int evaluateBoard(int player, Board board)
{
    int opp = 0;
    if (player == -1)
    {
        player = 0;
        opp = 1;
    }

    int fours = board.countStreaks(player, 4) / 2;
    int threes = board.countStreaks(player, 3) / 2;
    int twos = board.countStreaks(player, 2) / 2;
    int opp_fours = board.countStreaks(opp, 4) / 2;

    if (opp_fours > 0)
        return -100000;
    else
        return fours * 100000 + threes * 100 + twos;
}
