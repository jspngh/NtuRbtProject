#include <iostream>
#include <unistd.h>

#include "robot.hpp"

using namespace std;

int main(int, char**)
{
    Robot r;
    vector<int> calib_cols;
    calib_cols.push_back(3);
    calib_cols.push_back(0);
    calib_cols.push_back(1);
    calib_cols.push_back(2);
    calib_cols.push_back(4);
    calib_cols.push_back(5);
    calib_cols.push_back(6);
    r.calibBoard(calib_cols);
    return 0;
}
