#ifndef MINIMAX_H
#define MINIMAX_H

#include <string>

#include "../board.hpp"

#define DEBUG false
#define INF 99999999
#define FIRST_MOVE -1

#define MAX_DEPTH 8
#define MIN_DEPTH 2
#define DEFAULT_DEPTH 4

class Algorithm
{
    public:
        Algorithm(int depth=DEFAULT_DEPTH): mDepth(depth) {}

        void setDepth(int depth);
        void increaseDepth();
        void decreaseDepth();
        /**
         * Returns the best move (as a column number)
         *
         * Parameters:
         *      player:  0 -- Red
         *               1 -- Yellow
         */
        int nextMove(int player, Board board);

        int nextMovePython(int player, Board board);

        int evaluateBoard(int player, Board board);

    private:
        int mDepth;

        int negamax(int player, Board board, int depth);
        std::string exec(const char* cmd);
        void board_to_csv(Board board);
};
#endif

