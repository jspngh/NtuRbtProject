#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <string>
#include <vector>

#include "robot.hpp"

using namespace std;

Robot::Robot()
{
    server = new Server();
    lineSpeed = 50;
    ptpSpeed = 5;
}

Robot::~Robot()
{
    delete server;
}

Server* Robot::getServer()
{
    return server;
}

void Robot::dropPiece(int col, bool drop)
{
    // TODO make pretty
    col = 6 - col;
    float x = board_x0 + col * board_dx;
    float y = board_y;
    float z = board_z_hover;

    // hover
    RobotCoord position_hover = {x, y, z};
    move(position_hover);

    sleep(500);

    // slow down movement
    int cLineSpeed = lineSpeed, cPtpSpeed = ptpSpeed;
    setSpeed(30, 2);
    // drop
    RobotCoord position_drop = {x, y, board_z_drop};
    move(position_drop);
    if (drop)
        openGripper();
    // reset speed
    setSpeed(cLineSpeed, cPtpSpeed);

    sleep(500);

    move(position_hover);
}

void Robot::pickUpPiece()
{
    float x = piece_x;
    float y = piece_y;
    float z = piece_z_hover;

    // hover
    RobotCoord position_hover = {x, y, z};
    move(position_hover);

    sleep(500);

    // slow down movement
    int cLineSpeed = lineSpeed, cPtpSpeed = ptpSpeed;
    setSpeed(30, 2);
    // grab
    RobotCoord position_grab = {x, y, piece_z_grab};
    move(position_grab);
    closeGripper();
    move(position_hover);
    // reset speed
    setSpeed(cLineSpeed, cPtpSpeed);

    sleep(500);

    move(position_hover);
}

void Robot::setSpeed(int linespeed, int ptpspeed)
{
    lineSpeed = linespeed;
    ptpSpeed = ptpspeed;

    string command = "SETLINESPEED " + to_string(linespeed);
    const char* speed = command.c_str();
    server->sendCommand(speed, sockfd());

    command = "SETPTPSPEED " + to_string(ptpspeed);
    speed = command.c_str();
    server->sendCommand(speed, sockfd());
}

void Robot::resetJoints()
{
    string command = "MOVJ 0 45 -45 0 -90 90";
    const char* grip = command.c_str();
    server->sendCommand(grip, sockfd());
}

void Robot::goHome()
{
    string command = "GOHOME";
    const char* movt = command.c_str();
    server->sendCommand(movt, sockfd());
}

void Robot::calibBoard(vector<int> cols)
{
    setSpeed(lineSpeed, ptpSpeed);

    resetJoints();
    openGripper();
    sleep(3000);
    closeGripper();

    auto it = cols.begin();
    while (it != cols.end())
    {
        dropPiece(*it, false);
        it++;
    }
    resetJoints();
    openGripper();
}

void Robot::move(RobotCoord c)
{
    string command = "MOVP " + to_string((int)c.x) + " " + to_string((int)c.y) + " " + to_string((int)c.z) + " # # #";
    const char* movt = command.c_str();
    server->sendCommand(movt, sockfd());
}

void Robot::rotateGripper(double angle)
{
    this->resetJoints();

    string command = "MOVJ # # # # # " + to_string(angle);
    const char* grip = command.c_str();
    server->sendCommand(grip, sockfd());
}

void Robot::openGripper()
{
    string command = "OUTPUT 48 OFF";
    const char* open_grip = command.c_str();
    server->sendCommand(open_grip, sockfd());
}

void Robot::closeGripper()
{
    string command = "OUTPUT 48 ON";
    const char* close_grip = command.c_str();
    server->sendCommand(close_grip, sockfd());
}

void Robot::sleep(int msec)
{

    string command = "SLEEP " + to_string(msec);
    const char* sleep = command.c_str();
    server->sendCommand(sleep, sockfd());
}

int Robot::sockfd()
{
    int fd;
    if (server->client_sock > -1)
        fd = server->client_sock;
    else
        fd = server->openSocket();

    return fd;
}
