#include <vector>
#include "minimax.hpp"

using namespace std;

#define DEBUG false
struct Node
{
    Board board;
    int score;
    int move;
};

Node negamax(int player, Node node, int depth)
{
    //cout << endl;

    if (depth == 0 || node.board.isFinished())
    {
        node.score = evaluateBoard(player, node.board); // player * ??
        return node;
    }

    if(DEBUG) cout << "board 1: " << endl;
    if(DEBUG) cout << node.board << endl;

    Node best_node = {node.board, -99999999, -1};

    for (int col = 0; col < BOARD_WIDTH; col++)
    {
        if (node.board.isMoveLegal(col))
        {
            Node temp = node;
            temp.board.doMove(col, player);
            if(DEBUG) cout << "board 2: " << endl;
            if(DEBUG) cout << temp.board << endl;
            temp.move = col;
            Node n = negamax(-player, temp, depth - 1);
            n.score = - n.score;
            if(DEBUG) cout << "score " << n.score << endl;
            if(DEBUG) cout << "move " << n.move << endl;
            if (n.score > best_node.score)
                best_node = n;
        }
    }

    //cout << endl << "best move " << best_node.move << endl;
    return best_node;
}

// Pseudo code:
// function negamax(node, depth, color)
//     if depth = 0 or node is a terminal node
//         return color * the heuristic value of node

//     bestValue := −∞
//     foreach child of node
//         v := −negamax(child, depth − 1, −color)
//         bestValue := max( bestValue, v )
//     return bestValue
int nextMove(int player, Board board, int depth)
{
    Node temp = {board, -99999999, -1};
    Node n = negamax(player, temp, depth);
    return n.move;
}


// TODO: check correctness, the evaluation is independent of the player
// i.e. this value is always calculated from the point of view of player 1
int evaluateBoard(int player, Board board)
{
    int opp;
    if (player == 1)
        opp = 0;
    else
    {
        player = 0;
        opp = 1;
    }



    int fours = board.countStreaks(player, 4);
    int threes = board.countStreaks(player, 3);
    int twos = board.countStreaks(player, 2);
    int opp_fours = board.countStreaks(opp, 4);

    if (opp_fours > 0)
    {
        //cout << "opponent 4" << endl;
        return -100000;
    }
    else
        return fours * 100000 + threes * 100 + twos;


    //return (board.countStreaks(1, 4) - board.countStreaks(0, 4)) * 99999
            //+ (board.countStreaks(1, 3) - board.countStreaks(0, 3)) * 100
            //+ (board.countStreaks(1, 2) - board.countStreaks(0, 2)) * 10;
}
