#include <vector>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <cmath>

#include "minimax.hpp"

using namespace std;


struct Node
{
    int score;
    int move;
};


void Algorithm::setDepth(int depth)
{
    this->mDepth = depth;
}


void Algorithm::increaseDepth()
{
    this->mDepth = max(MIN_DEPTH, this->mDepth - 1);
}


void Algorithm::decreaseDepth()
{
    this->mDepth = min(MAX_DEPTH, this->mDepth + 1);
}


int Algorithm::negamax(int player, Board board, int depth)
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

int Algorithm::nextMove(int player, Board board)
{
    vector<Node> legal_moves;
    for (int col = 0; col < BOARD_WIDTH; col++)
    {
        if (board.isMoveLegal(col))
        {
            Board temp = board;
            temp.doMove(col, player);
            int score = - negamax(-player, temp, mDepth - 1);
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

int Algorithm::nextMovePython(int player, Board board)
{
    board_to_csv(board);
    string col = exec("python2 ./algorithm/minimax.py");
    return stoi(col);
}


int Algorithm::evaluateBoard(int player, Board board)
{
    int opp = 0;
    if (player == -1 || player == 0)
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

void Algorithm::board_to_csv(Board board)
{
    ofstream file;
    file.open ("./algorithm/board.csv");
    for (int row = 0; row < BOARD_HEIGHT; row++)
    {
        for (int col = 0; col < BOARD_WIDTH - 1; col++)
        {
            file << board.getState(row, col) << ",";
        }

        file << board.getState(row, BOARD_WIDTH - 1) << "\n";
    }
    file.close();
}

string Algorithm::exec(const char* cmd)
{
    char buffer[128];
    std::string result = "";
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}
