#include <string>

#include "board.hpp"


using namespace std;


Board::Board()
{
    this->board = new State*[BOARD_HEIGHT];
    for (int row = 0; row < BOARD_HEIGHT; row++)
    {
        this->board[row] = new State[BOARD_WIDTH];
        for (int col = 0; col < BOARD_WIDTH; col++)
        {
            this->board[row][col] = State::red;
        }
    }
}

Board::~Board()
{
    for (int row = 0; row < BOARD_HEIGHT; row++)
    {
        delete this->board[row];
    }
    delete this->board;
}



ostream& operator<<(ostream& os, const Board& b)
{
    os<<" 1   2   3   4   5   6   7\n";
    os<<"--------------------------\n";
    for(int row = 0; row < BOARD_HEIGHT; row++)
    {
        for(int col = 0; col < BOARD_WIDTH; col++)
        {
            string append = " |";
            if (col == BOARD_WIDTH - 1)
                append = "";

            os << " " << b.state2str[b.board[row][col]] << append;
        }
        os<<"\n--------------------------\n";
    }
    return os;
}
