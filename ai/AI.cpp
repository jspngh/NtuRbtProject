#include "AI.h"

AI::AI(Robot* robot)
{
    mRobot = robot;
}

void AI::userMove(Board& b)
{

}

void AI::doMove(int col)
{
    mRobot->pickUpPiece();
    mRobot->dropPiece(col);
}
