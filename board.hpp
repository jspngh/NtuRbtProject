#ifndef BOARD_H
#define BOARD_H


#define BOARD_HEIGHT 6
#define BOARD_WIDTH  7

enum State { red, yellow, empty };

class Board
{
    public:
        Board();
        ~Board(){};
        State getState(int row, int col);
        setState(int row, int col, State state);
        State* getBoard();
    private:
        State **board;
};


#endif
