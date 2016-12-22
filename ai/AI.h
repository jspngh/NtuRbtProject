#include <thread>
#include <netinet/in.h>
#include <string>

#include "../board.hpp"
#include "../robot/robot.hpp"

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
    enum BehaviourState
    {
        LOSING          = 0b00000001,
        WINNING         = 0b00000010,
        BUSY_MOVE       = 0b00000100,
        WAITING_VISION  = 0b00001000,
        WAITING_USER    = 0b00010000,
        BOARD_PROC_ERR  = 0b00100000,
    };

    enum VoiceCommand
    {
        HELLO = 0,
        HOW_ARE_YOU = 1,
        COMPLIMENT = 2,
        INSULT = 3,
        TOO_EASY = 4,
        TOO_HARD = 5,
    };

    AI(Robot* robot, S2Tcomm c);

    // in different thread
    void processVoice();
    void processVisual();
    void processCmd(VoiceCommand cmd);

    void userMove(Board& b);
    void doMove(int col);

    bool isWinning();
    bool isLosing();
    bool isWaiting();
    bool procErr();
private:
    Robot* mRobot;
    Board mBoard;

    S2Tcomm mComm;
    std::thread mVoiceThread;
    bool mProcVoice;
    int mBehaviour;

    bool initS2Tcommunication();
    bool isState(BehaviourState s);
};
