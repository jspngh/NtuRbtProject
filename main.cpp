#include <unistd.h>
#include <iostream>
#include <ctime>

#include "board.hpp"
#include "minimax.hpp"
#include "vision/VisionManager.h"
#include "robot/robot.hpp"
#include "ai/AI.h"

using namespace std;

void getUserMove(Board& b, VisionManager& vm)
{
    // user move
    // cout << "give a column please (1-7): ";
    // cin >> col;
    // col = col - 1;
    // b.doMove(col, player_user);

    cout << "it's your turn!" << endl;
    cv::waitKey(20000);

    bool success = false;
    for (int i=0; i<4 && !success; i++)
    {
        success = vm.updateBoard(b);
    }

    if (!success)
    {
        //TODO: do some interaction with the user
        cout << "ERR: could not get a correct user move, exiting..." << endl;
        exit(-1);
    }

    cout << "player made his move" << endl;
    cout << b << endl;
}

void time_AI(int N)
{
    cout << "N,time" << endl;
    int player_user = 0;
    int player_ai = 1;
    for (int depth = 1; depth <= N; depth++)
    {
        Board b;

        // random user move
        b.doMove(4, player_user);

        // time the duration of the AI calculations
        clock_t begin = clock();
        int col = nextMove(player_ai, b, depth);
        clock_t end = clock();

        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        cout << depth << "," << elapsed_secs << endl;
    }
}


int main (int argc, char* args[])
{
    Board b;
    Robot* robot = new Robot();
    AI ai(robot);

    Freenect::Freenect freenect;
    KinectManager& device = freenect.createDevice<KinectManager>(0);
    VisionManager vm(&device);
    if (!vm.initVision())
    {
        cout << "ERR: could not initialize Kinect, exiting..." << endl;
        exit(-1);
    }


    int player_user = 0;
    int player_ai = 1;
    int col;
    int depth = 6;

    int winner = -1;

    while (winner == -1)
    {
        getUserMove(b, vm);

        // check if user won
        if (b.getWinner() != -1)
        {
            winner = player_user;
            break;
        }

        // AI's move
        col = nextMovePython(player_ai, b, depth);
        cout << "next move: " << col << endl;
        b.doMove(col, player_ai);
        ai.doMove(col);

        // check if AI won
        if (b.getWinner() != -1)
        {
            winner = player_ai;
            break;
        }

        // print board
        cout << b << endl;
    }

    cout << b << endl;

    cout << "game is finished" << endl;
    cout << "winner: " << winner << endl;
    vm.stopVision();
}
