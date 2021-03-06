#include <unistd.h>
#include <iostream>
#include <ctime>
#include <random>

#include "board.hpp"
#include "algorithm/minimax.hpp"
#include "vision/VisionManager.h"
#include "robot/robot.hpp"
#include "hci/HCI.h"
#include "ai/AI.h"

using namespace std;

Algorithm algorithm;
Robot* robot;
HCI* hci;

int player_user = 0;
int player_ai = 1;

void getUserMove(Board& b, VisionManager& vm, AI& ai);
void waitRobotMove(Board& b, VisionManager& vm, AI& ai, int expected_col);

int main (int argc, char* args[])
{
    Board b;
    hci = new HCI();
    robot = new Robot();
    S2Tcomm c;
    AI ai(robot, hci, c, algorithm, &b);

    Freenect::Freenect freenect;
    KinectManager& device = freenect.createDevice<KinectManager>(0);
    VisionManager vm(&device);
    if (!vm.initVision())
    {
        cout << "ERR: could not initialize Kinect, exiting..." << endl;
        exit(-1);
    }

    int col;
    int depth = 6;

    int winner = -1;
    bool first_move = true;

    sleep(5);
    ai.processState(INVITE);

    while (winner == -1)
    {
        sleep(5);
        if (first_move)
        {
            ai.processState(BEGIN_GAME);
            sleep(5);
        }

        getUserMove(b, vm, ai);

        // check if user won
        if (b.getWinner() != -1)
        {
            winner = player_user;
            break;
        }

        // AI's move
        if (rand() % 3 == 0 || first_move)
        {
            ai.processState(MOVE_BEGIN);
            first_move = false;
        }

        col = ai.doMove();
        waitRobotMove(b, vm, ai, col);
        // check if AI won
        if (b.getWinner() != -1)
        {
            winner = player_ai;
            break;
        }
        ai.processState(MOVE_DONE);

        // print board
        cout << b << endl;
    }

    cout << b << endl;

    if (winner == player_ai)
        ai.processState(WON);
    else
        ai.processState(LOST);
    sleep(10);

    cout << "game is finished" << endl;
    cout << "winner: " << winner << endl;
    vm.stopVision();
}

void getUserMove(Board& b, VisionManager& vm, AI& ai)
{
    cout << "Player, it's your turn now!" << endl;

    Board doublecheck_board = b.getDeepCopy();
    Board reset_board = b.getDeepCopy();

    bool stop = false;
    VisionResult r;
    int attempts = 3;
    while(!stop && attempts > 0)
    {
        r = vm.updateBoard(b);
        switch (r)
        {
            case NO_MOVE:
                cout << "no move done" << endl;
                sleep(5);
                break;
            case PROC_ERR:
                cout << "vision processing error" << endl;
                attempts--;
                sleep(1);
                break;
            case SUCCESS:
                stop = true;
                sleep(1);
                r = vm.updateBoard(doublecheck_board);
                for (int i=0; i<BOARD_HEIGHT; i++)
                {
                    for (int j=0; j<BOARD_WIDTH; j++)
                    {
                        if (doublecheck_board.getState(i,j) != b.getState(i,j))
                        {
                            cout << "ERR: user move was detected wrongly" << endl;
                            stop = false;
                            i = BOARD_HEIGHT;
                            j = BOARD_WIDTH;
                            b = reset_board.getDeepCopy();
                            doublecheck_board = reset_board.getDeepCopy();
                        }
                    }
                }
                break;
        }
    }

    if (r == PROC_ERR)
    {
        ai.processState(BOARD_PROC_ERR);
        cout << b << endl;
        cout << "ERR: could not get a correct user move, exiting..." << endl;
        exit(-1);
    }

    if (rand() % 5 == 0)
        ai.processState(COMPLIMENT);

    cout << "player made his/her move" << endl;
    cout << b << endl;
}

void waitRobotMove(Board& b, VisionManager& vm, AI& ai, int expected_col)
{
    cout << "Robot is making it's move" << endl;

    State expected_board[BOARD_HEIGHT][BOARD_WIDTH];
    for (int i=BOARD_HEIGHT-1; i>-1; i--)
    {
        for (int j=0; j<BOARD_WIDTH; j++)
        {
            if (j == expected_col && b.getState(i,j) == Empty)
            {
                // TODO: do this better and modular
                expected_board[i][j] = Red;
                expected_col = -1;
            }
            else
            {
                expected_board[i][j] = b.getState(i,j);
            }
        }
    }
    for (int i=0; i<BOARD_HEIGHT; i++)
    {
        for (int j=0; j<BOARD_WIDTH; j++)
        {
            cout << expected_board[i][j] << " ";
        }
        cout << endl;
    }

    bool stop = false;
    VisionResult r;
    int attempts = 3;
    while(!stop && attempts > 0)
    {
        r = vm.updateBoard(b);
        switch (r)
        {
            case NO_MOVE:
                cout << "no move done" << endl;
                sleep(5);
                break;
            case PROC_ERR:
                cout << "vision processing error" << endl;
                attempts--;
                sleep(1);
                break;
            case SUCCESS:
                stop = true;
                break;
        }
    }

    if (r == PROC_ERR)
    {
        ai.processState(BOARD_PROC_ERR);
        cout << b << endl;
        cout << "ERR: could not get the correct robot move, exiting..." << endl;
        exit(-1);
    }

    for (int i=0; i<BOARD_HEIGHT; i++)
    {
        for (int j=0; j<BOARD_WIDTH; j++)
        {
            if (expected_board[i][j] != b.getState(i,j))
            {
                cout << "ERR: wrong move detected" << endl;
                cout << b << endl;
                exit(-1);
            }
        }
    }

    cout << "robot made its move" << endl;
}
