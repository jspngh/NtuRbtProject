#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>

#include "AI.h"

#define MOCK_VOICE

using namespace std;

AI::AI(Robot* robot, HCI* hci, S2Tcomm c, Algorithm& algo, Board& board):
    mVoiceThread(&AI::processVoice, this),
    mRobot(robot),
    mHCI(hci),
    mComm(c),
    mAlgorithm(algo),
    mBoard(board)
{
    mBehaviour = 0;
    mProcVoice = true;
}

void AI::processVoice()
{
    char buffer[256];
    socklen_t cl_len;
    struct sockaddr_in svr_addr, cl_addr;

#ifdef MOCK_VOICE
    while (true)
    {
        usleep(10000000);
        processCmd(HOW_ARE_YOU);
    }
    return;
#endif

    if (!initS2Tcommunication())
    {
        cout << "stopping voice thread..." << endl;
        return;
    }

    while (mProcVoice)
    {
        listen(mComm.serverfd,5);
        cl_len = sizeof(cl_addr);
        mComm.sockfd = accept(mComm.serverfd,
                (struct sockaddr *) &cl_addr,
                &cl_len);
        if (mComm.sockfd< 0)
        {
            cout << "ERROR on accept" << endl;
            break;
        }

        mComm.sockfd = mComm.sockfd;
        bzero(buffer,256);
        int n = read(mComm.sockfd, buffer, 255);
        if (n < 0)
        {
            cout << "ERROR reading from socket" << endl;
            close(mComm.sockfd);
            break;
        }

        VoiceCommand received_cmd = static_cast<VoiceCommand>(atoi(buffer));
        printf("I have received: %d\n", received_cmd);
        close(mComm.sockfd);

        processCmd(received_cmd);
    }

    cout << "stopping voice thread..." << endl;
    close(mComm.serverfd);
}

void AI::processCmd(VoiceCommand cmd)
{
    // TODO see below
    printf("Processing command: %d\n", cmd);

    switch (cmd)
    {
        case TOO_EASY:
            cout << "TOO EASY" << endl;
            // go to soundboard and response
            // in the sound board select random reply in correct category
            // visualize response
            mHCI->msg({0, cmd});
            // adjust level
            mAlgorithm.increaseDepth();
            break;
        case TOO_HARD:
            cout << "TOO HARD" << endl;
            // go to soundboard and response
            // in the sound board select random reply in correct category
            // visualize response
            mHCI->msg({0, cmd});
            // adjust level
            mAlgorithm.decreaseDepth();
            break;
        default:
            // go to soundboard and response
            // in the sound board select random reply in correct category
            // visualize response
            Message tmp = {0, cmd};
            printf("Sending command\n");
            mHCI->msg(tmp);
    }

}

void AI::processState(BehaviourState state)
{
    printf("Processing behavior state: %d\n", state);
    Message tmp = {1, {.s = state}};
    printf("Sending command\n");
    mHCI->msg(tmp);
}

void AI::processVisual()
{

}

int AI::doMove()
{
    int player_ai = 1;
    int col = mAlgorithm.nextMovePython(player_ai, mBoard);
    cout << "AI selected col: " << col << endl;

    mRobot->resetJoints();
    mRobot->pickUpPiece();
    mRobot->dropPiece(col);
    mRobot->resetJoints();

    return col;
}

bool AI::isWinning()
{
    return isState(WINNING) && !isState(LOSING);
}

bool AI::isLosing()
{
    return isState(LOSING) && !isState(WINNING);
}

bool AI::isWaiting()
{
    return isState(BUSY_MOVE)
        || isState(WAITING_USER)
        || isState(WAITING_VISION);
}

bool AI::procErr()
{
    return isState(BOARD_PROC_ERR);
}

bool AI::initS2Tcommunication()
{
    int svr_sockfd, cl_sockfd;
    socklen_t cl_len;
    struct sockaddr_in svr_addr, cl_addr;

    svr_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr_sockfd < 0)
    {
        cout << "ERROR opening socket" << endl;
        return false;
    }

    bzero((char *) &svr_addr, sizeof(svr_addr));
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(mComm.port);

    if (bind(svr_sockfd,
             (struct sockaddr *) &svr_addr,
             sizeof(svr_addr)) < 0)
    {
        cout << "ERROR on binding" << endl;
        return false;
    }
    mComm.serverfd = svr_sockfd;
    return true;
}

bool AI::isState(BehaviourState s)
{
    bool result = (mBehaviour & s) > 0 ? true : false;
    return result;
}
