#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <ostream>

#define BOARD_HEIGHT 6
#define BOARD_WIDTH  7

enum State { Red=0, Yellow, Empty };


class Board
{
    public:
        Board();
        ~Board();
        State getState(int row, int col);
        void setState(int row, int col, State state);
        //State[][] getBoard();
        bool isMoveLegal(int col);
        bool doMove(int col, int player);
        friend std::ostream& operator<<(std::ostream& os, const Board& board);
    private:
        State board[BOARD_HEIGHT][BOARD_WIDTH];
        const char* state2str[3] = { "X", "O", " "};
};


#endif
