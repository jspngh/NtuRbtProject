#include <string>

#include "board.hpp"


using namespace std;


Board::Board()
{
    for (int row = 0; row < BOARD_HEIGHT; row++)
    {
        for (int col = 0; col < BOARD_WIDTH; col++)
        {
            board[row][col] = State::Empty;
        }
    }
}

Board::~Board()
{
}


bool Board::isMoveLegal(int col)
{
    return board[0][col] == State::Empty;
}

bool Board::doMove(int col, int player)
{
    if (isMoveLegal(col))
    {
        int row = BOARD_HEIGHT - 1;
        // look for the first empty spot in the column
        for (; row >= 0 && board[row][col] != State::Empty; row--);
        board[row][col] = (player == 0 ? State::Red : State::Yellow);
        return true;
    }
    return false;
}

ostream& operator<<(ostream& os, const Board& b)
{
    os << endl << b.colNumbers << endl;
    os << b.delimeter << endl;
    for(int row = 0; row < BOARD_HEIGHT; row++)
    {
        for(int col = 0; col < BOARD_WIDTH; col++)
        {
            string append = " |";
            if (col == BOARD_WIDTH - 1)
                append = "\n";

            os << " " << b.state2str[b.board[row][col]] << append;
        }
    }
    return os;
}

int Board::gameFinished()
{
    if (countStreaks(0, 4) > 0)
        return 0;

    if (countStreaks(1, 4) > 0)
        return 1;

    return -1;
}

int Board::countStreaks(int player, int streak)
{
    int count = 0;
    for (int row = 0; row < BOARD_HEIGHT; row++)
    {
        for (int col = 0; col < BOARD_WIDTH; col++)
        {
            count += checkHorizontal(row, col, player, streak);
            count += checkVertical(row, col, player, streak);
            count += checkDiagonal(row, col, player, streak);
        }
    }
    return count;
}

bool Board::isInside(int row, int col)
{
    return col >= 0 && col < BOARD_WIDTH && row >= 0 && row < BOARD_HEIGHT;
}

int Board::checkHorizontal(int row, int col, int player, int streak)
{
    return checkStreak(row, col, 0, 1, player, streak)
            + checkStreak(row, col, 0, -1, player, streak);
}

int Board::checkVertical(int row, int col, int player, int streak)
{
    return checkStreak(row, col, 1, 0, player, streak)
            + checkStreak(row, col, -1, 0, player, streak);
}

int Board::checkDiagonal(int row, int col, int player, int streak)
{
    return checkStreak(row, col, 1, 1, player, streak)
            + checkStreak(row, col, -1, -1, player, streak)
            + checkStreak(row, col, -1, 1, player, streak)
            + checkStreak(row, col, 1, -1, player, streak);
}

int Board::checkStreak(int row, int col, int upDir, int rightDir, int player, int streak)
{
    for (int i = 0; i < streak; i++)
    {
        if (!isInside(row, col))
            return 0;

        if (board[row][col] != player)
            return 0;

        col += rightDir;
        row += upDir;
    }
    return 1;
}

//int heur_point(int x, int y)
        //{
            //int score=0; i=0; aux=1;
            //int max,min;
            //std::vector<int> p;
            //std::vector<int> r;


            ////Left to right Diagonal
            //if (((x+1)<7)&&((y-1)>-1))
            //{
                //if (arr[x+1,y-1]==rbt)
                //{
                    //r[1]++;
                    //if (((x+2)<8)&&((y-2)>-1))
                    //{
                        //if (arr[x+2,y-2]==rbt)
                        //{
                            //r[1]++;
                            //if (((x+3)<7)&&((y-3)>-1))
                            //{
                                //if (arr[x+3,y-3]==rbt)
                                    //r[1]++;}}}}
                //}
            //if (((x-1)>-1)&&((y+1)<7))
            //{
                //if (arr[x-1,y+1]==rbt)
                //{
                    //r[1]++;
                    //if (((x-2)>-1)&&((y+2)<7))
                    //{
                        //if (arr[x-2,y+2]==rbt)
                        //{
                            //r[1]++;
                            //if (((x-3)>-1)&&((y+3)<7))
                            //{
                                //if (arr[x-3,y+3]==rbt)
                                    //r[1]++;}}}}
                //}

            ////Right to left Diagonal
            //if (((x-1)>-1)&&((y-1)>-1))
            //{
                //if (arr[x-1,y-1]==rbt)
                //{
                    //r[2]++;
                    //if (((x-2)>-1)&&((y-2)>-1))
                    //{
                        //if (arr[x-2,y-2]==rbt)
                        //{
                            //r[2]++;
                            //if (((x-3)>-1)&&((y-3)>-1))
                            //{
                                //if (arr[x-3,y-3]==rbt)
                                    //r[2]++;}}}}
                //}
            //if (((x+1)<7)&&((y+1)<7))
            //{
                //if (arr[x+1,y+1]==rbt)
                //{
                    //r[2]++;
                    //if (((x+2)<7)&&((y+2)<7))
                    //{
                        //if (arr[x+2,y+2]==rbt)
                        //{
                            //r[2]++;
                            //if (((x+3)<7)&&((y+3)<7))
                            //{
                                //if (arr[x+3,y+3]==rbt)
                                    //r[2]++;}}}}
                //}


            ////Vertical
            //if ((y-1)>-1)if (arr[x,y-1]==rbt)
                         //{
                             //r[0]++;
                             //if ((y-2)>-1)if (arr[x,y-2]==rbt)
                                          //{
                                              //r[0]++;
                                              //if ((y-3)>-1)if (arr[x,y-3]==rbt)
                                                //r[0]++;}
                                          //}


            ////Horizontal
            //if (x-1>-1)
            //{
                //if (arr[x-1,y]==rbt)
                //{
                    //r[3]++;
                    //if (x-2>-1)
                    //{
                        //if (arr[x-2,y]==rbt)
                        //{
                            //r[3]++;
                            //if (x-3>-1)if (arr[x-3,y]==rbt)
                                //r[3]++;}}}
            //}
            //if (x+1<7)
            //{
                //if (arr[x+1,y]==rbt)
                //{
                    //r[3]++;
                    //if (x+2<7)
                    //{
                        //if (arr[x+2,y]==rbt)
                        //{
                            //r[3]++;
                            //if (x+3<7)if (arr[x+3,y]==rbt)
                                //r[3]++;}}}
            //}

            //max=r[0];
            //if (r[1]>max)max=r[1];
            //if (r[2]>max)max=r[2];
            //if (r[3]>max)max=r[3];

            //max =10^max;


            ////Left to right diagonal
            //if (((x+1)<7)&&((y-1)>-1))
            //{
                //if (arr[x+1,y-1]==plr)
                //{
                    //p[1]++;
                    //if (((x+2)<7)&&((y-2)>-1))
                    //{
                        //if (arr[x+2,y-2]==plr)
                        //{
                            //p[1]++;
                            //if (((x+3)<7)&&((y-3)>-1))
                            //{
                                //if (arr[x+3,y-3]==plr)
                                    //p[1]++;}}}}
            //}

            //if (((x-1)>-1)&&((y+1)<7))
            //{
                //if (arr[x-1,y+1]==plr)
                //{
                    //p[1]++;
                    //if (((x-2)>-1)&&((y+2)<7))
                    //{
                        //if (arr[x-2,y+2]==plr)
                        //{
                            //p[1]++;
                            //if (((x-3)>-1)&&((y+3)<7))
                            //{
                                //if (arr[x-3,y+3]==plr)
                                    //p[1]++;}}}}
                //}


            ////Right to left diagonal
            //if (((x-1)>-1)&&((y-1)>-1))
            //{
                //if (arr[x-1,y-1]==plr)
                //{
                    //p[2]++;
                    //if (((x-2)>-1)&&((y-2)>-1))
                    //{
                        //if (arr[x-2,y-2]==plr)
                        //{
                            //p[2]++;
                            //if (((x-3)>-1)&&((y-3)>-1))
                            //{
                                //if (arr[x-3,y-3]==plr)
                                    //p[2]++;}}}}
                //}
            //if (((x+1)<7)&&((y+1)<7))
            //{
                //if (arr[x+1,y+1]==plr)
                //{
                    //p[2]++;
                    //if (((x+2)<7)&&((y+2)<7))
                    //{
                        //if (arr[x+2,y+2]==plr)
                        //{
                            //p[2]++;
                            //if (((x+3)<7)&&((y+3)<7))
                            //{
                                //if (arr[x+3,y+3]==plr)
                                    //p[2]++;}}}}
            //}


            ////Vertical
            //if ((y-1)>-1)if (arr[x,y-1]==plr)
                        //{
                             //p[0]++;
                             //if ((y-2)>-1)if (arr[x,y-2]==plr)
                                          //{
                                              //p[0]++;
                                              //if ((y-3)>-1)if (arr[x,y-3]==plr)
                                                //p[0]++;}
                        //}


            ////Horizontal
            //if (x-1>-1)
            //{
                //if (arr[x-1,y]==plr)
                //{
                    //p[3]++;
                    //if (x-2>-1)
                    //{
                        //if (arr[x-2,y]==plr)
                        //{
                            //p[3]++;
                            //if (x-3>-1)if (arr[x-3,y]==plr)
                                //p[3]++;}}}
                //}
            //if (x+1<7)
            //{
                //if (arr[x+1,y]==plr)
                //{
                    //p[3]++;
                    //if (x+2<7)
                    //{
                        //if (arr[x+2,y]==plr)
                        //{
                            //p[3];
                            //if (x+3<7)if (arr[x+3,y]==plr)
                                //p[3];}}}
                //}




            //min=p[0];
            //if (p[1]>min)min=p[1];
            //if (p[2]>min)min=p[2];
            //if (p[3]>min)min=p[3];

            //min = 10^min;


            ////Evaluating situations on the board - Minmax

                //if(min==max) max=3*max;
                //score = max-min;

        //return score;

        //}



        //int checkwin()
        //{
            //int r,x,y;
            //r=0;
            //for(y=2;y>-1;y--)
            //{
                //for(x=0;x<7;x++)
                //{
                    //vert(x,y,ref r);
                //}
            //}
            //for(y=0;y<7;y++)p
            //{
                //for(x=0;x<4;x++)
                //{
                    //horiz(x,y,ref r);
                //}
            //}
            //for(y=2;y>-1;y--)
            //{
                //for(x=0;x<4;x++)
                //{
                    //diar(x,y,ref r);
                //}
            //}
            //for(y=2;y>-1;y--)
            //{
                //for(x=3;x<7;x++)
                //{
                    //dial(x,y,ref r);
                //}
            //}
            //return r;
        //}

        //void vert(int x,int y,ref int r)
        //{
            //if ((arr[x,y]==2)&&(arr[x,y+1]==2)&&(arr[x,y+2]==2)&&(arr[x,y+3]==2))r=2;
            //if ((arr[x,y]==1)&&(arr[x,y+1]==1)&&(arr[x,y+2]==1)&&(arr[x,y+3]==1))r=1;
        //}

        //void horiz(int x,int y,ref int r)
        //{
            //if ((arr[x,y]==2)&&(arr[x+1,y]==2)&&(arr[x+2,y]==2)&&(arr[x+3,y]==2))r=2;
            //if ((arr[x,y]==1)&&(arr[x+1,y]==1)&&(arr[x+2,y]==1)&&(arr[x+3,y]==1))r=1;
        //}

        //void diagr(int x,int y,ref int r)
        //{
            //if ((arr[x,y]==2)&&(arr[x+1,y+1]==2)&&(arr[x+2,y+2]==2)&&(arr[x+3,y+3]==2))r=2;
            //if ((arr[x,y]==1)&&(arr[x+1,y+1]==1)&&(arr[x+2,y+2]==1)&&(arr[x+3,y+3]==1))r=1;
        //}

        //void diagl(int x,int y,ref int r)
        //{
            //if ((arr[x,y]==2)&&(arr[x-1,y+1]==2)&&(arr[x-2,y+2]==2)&&(arr[x-3,y+3]==2))r=2;
            //if ((arr[x,y]==1)&&(arr[x-1,y+1]==1)&&(arr[x-2,y+2]==1)&&(arr[x-3,y+3]==1))r=1;
        //}



