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

AI::AI(Robot* robot, S2Tcomm c) :
    mVoiceThread(&AI::processVoice, this),
    mRobot(robot),
    mComm(c)
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

        printf("I have received: %d\n", atoi(buffer));
        close(mComm.sockfd);
    }

    cout << "stopping voice thread..." << endl;
    close(mComm.serverfd);
}

void AI::processVisual()
{

}

void AI::userMove(Board& b)
{

}

void AI::doMove(int col)
{
    // mRobot->pickUpPiece();
    // mRobot->dropPiece(col);
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
