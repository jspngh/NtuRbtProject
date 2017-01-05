#include <iostream>
#include <unistd.h>

#include "robot.hpp"

using namespace std;

int main(int, char**)
{
    Robot *r = new Robot();
    r->setSpeed(50, 5);
    r->resetJoints();
    r->pickUpPiece();
    r->resetJoints();
    vector<int> calib_cols;
    calib_cols.push_back(3);
    calib_cols.push_back(0);
    calib_cols.push_back(6);
    calib_cols.push_back(3);
    calib_cols.push_back(0);
    calib_cols.push_back(6);
    r->calibBoard(calib_cols);
    return 0;
}
