#ifndef M_ROBOT
#define M_ROBOT

#include <vector>

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

        void dropPiece(int col, bool drop=true);
        void pickUpPiece();

        void setSpeed(int linespeed, int ptpspeed);
        void resetJoints();
        void goHome();
        void calibBoard(std::vector<int> cols);
        void openGripper();
        void closeGripper();

    private:
        void move(RobotCoord c);
        void rotateGripper(double angle);
        void sleep(int msec);

        int sockfd();

        Server* server;

        int lineSpeed;
        int ptpSpeed;

        // constants
        const int slowLineSpeed = 30;
        const int slowPtpSpeed = 2;

        const float board_x0 = 70;
        const float board_dx = -33;
        const float board_z_drop = -44;
        const float board_z_hover = 0;
        const float board_y = 450;

        const float piece_x = 205;
        const float piece_z_grab = -225;
        const float piece_z_hover = 0;
        const float piece_y = 450;
};

#endif
