#ifndef M_ROBOT
#define M_ROBOT

#include "server.hpp"

// 140.112.91.192

struct RobotCoord
{
    float x;
    float y;
    float z;
};

class Robot
{
    public:
        Robot();
        ~Robot();

        Server* getServer();

        void dropPiece(int col);

        void setSpeed(int linespeed, int ptpspeed);
        void resetJoints();
        void goHome();
        void openGripper();
        void closeGripper();
        void sleep(int msec);

    private:
        void move(RobotCoord c);
        void rotateGripper(double angle);

        Server* server;

        // constants
        const float board_x0 = 99;
        const float board_dx = -33;
        const float board_z_drop = -48;
        const float board_z_hover = 0;
        const float board_y = 450;
};

#endif
