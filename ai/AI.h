#ifndef M_AI
#define M_AI

#include <thread>
#include <netinet/in.h>
#include <string>

#include "Message.h"
#include "../board.hpp"
#include "../robot/robot.hpp"
#include "../hci/HCI.h"
#include "../algorithm/minimax.hpp"

struct S2Tcomm
{
    int buflen = 512;
    int port = 50007;

    int sockfd = -1;
    int serverfd = -1;
};

class AI
{
public:
    AI(Robot* robot, HCI* hci, S2Tcomm c, Algorithm& algorithm, Board* board);

    // in different thread
    void processVoice();
    void processVisual();
    void processCmd(VoiceCommand cmd);

    void userMove(Board& b);
    int doMove();

    bool isWinning();
    bool isLosing();
    bool isWaiting();
    bool procErr();
private:
    Robot* mRobot;
    HCI* mHCI;
    Board* mBoard;
    Algorithm mAlgorithm;

    S2Tcomm mComm;
    std::thread mVoiceThread;
    bool mProcVoice;
    int mBehaviour;

    bool initS2Tcommunication();
    bool isState(BehaviourState s);
};

#endif
