#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <string>

#define BOARD_HEIGHT 6
#define BOARD_WIDTH  7

enum State { Red=0, Yellow=1, Empty };


class Board
{
    public:
        Board();
        ~Board();
        Board& operator=(const Board& b)
        {
            for(int row = 0; row < BOARD_HEIGHT; row++)
                for(int col = 0; col < BOARD_WIDTH; col++)
                    board[row][col] = b.board[row][col];

            return *this;
        }

        State getState(int row, int col);
        void setState(int row, int col, State state);
        State** getBoardCopy();
        Board getDeepCopy();

        bool isMoveLegal(int col);
        bool doMove(int col, int player);

        /**
         *  Returns the number of streaks of length streak for a player
         *  Parameters:
         *          player -- O or 1, for resp. Red or Yellow player
         *          streak -- length of the streak (maximum 4)
         */
        int countStreaks(int player, int streak);

        /**
         * Return:
         *         -1  -- if the game is not finished
         *          0  -- if player 0 (Red) has won
         *          1  -- if player 1 (Yellow) has won
         */
        int getWinner();
        bool isFinished();

        friend std::ostream& operator<<(std::ostream& os, const Board& board);
    private:
        bool isInside(int row, int col);

        /**
         *  Returns the number of horizontal streaks of certain length
         *  for a player that start at (row, col) position.
         *  Parameters:
         *          row -- integer between 0 and BOARD_HEIGHT
         *          col -- integer between 0 and BOARD_WIDTH
         *          player -- O or 1, for resp. Red or Yellow player
         *          streak -- length of the streak (maximum 4)
         */
        int checkHorizontal(int row, int col, int player, int streak);

        /** Same as checkHorizontal but for vertical streaks*/
        int checkVertical(int row, int col, int player, int streak);

        /** Same as checkHorizontal but for diagonal streaks going up*/
        int checkDiagonal(int row, int col, int player, int streak);

        /**
         *  Parameters:
         *          upDir -- -1 down, 0 don't move vertically, 1 move up
         *          rightDir -- -1 left, 0 don't move horizontally, 1 right
         */
        int checkStreak(int row, int col, int upDir, int rightDir, int player, int streak);

        // Members
        State board[BOARD_HEIGHT][BOARD_WIDTH];

        // some constants for printing
        const char* state2str[3] = { "R", "Y", " "};
        const std::string colNumbers = " 1   2   3   4   5   6   7";
        const std::string delimeter = "--------------------------";
};


#endif
