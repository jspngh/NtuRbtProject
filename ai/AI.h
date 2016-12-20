#include "../board.hpp"
#include "../robot/robot.hpp"

class AI
{
public:
    enum BehaviourState
    {
        LOSING,
        WINNING,
        BUSY_MOVE,
        WAITING_VISION,
        WAITING_USER,
        BOARD_PROC_ERR,
    };

    AI(Robot* robot);

    // in different thread
    void processVoice();
    void processVisual();

    void userMove(Board& b);
    void doMove(int col);

private:
    BehaviourState mState;
    Robot* mRobot;
    Board mBoard;
};
