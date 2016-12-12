#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <ostream>

#define BOARD_HEIGHT 6
#define BOARD_WIDTH  7

enum State { red=0, yellow, empty };


class Board
{
    public:
        Board();
        ~Board();
        State getState(int row, int col);
        void setState(int row, int col, State state);
        State* getBoard();
	friend std::ostream& operator<<(std::ostream& os, const Board& board);
    private:
        State **board;
	const char* state2str[3] = { "X", "O", " "};
};


#endif
