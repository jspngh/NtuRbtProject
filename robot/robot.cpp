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
}

Robot::~Robot()
{
    delete server;
}

Server* Robot::getServer()
{
    return server;
}

void Robot::setSpeed(int linespeed, int ptpspeed)
{
    int sockfd;
    if (server->client_sock > -1)
        sockfd = server->client_sock;
    else
        sockfd = server->openSocket();

    string command = "SETLINESPEED " + to_string(linespeed);
    const char* speed = command.c_str();
    server->sendCommand(speed, sockfd);

    command = "SETPTPSPEED " + to_string(ptpspeed);
    speed = command.c_str();
    server->sendCommand(speed, sockfd);
}

void Robot::move(RobotCoord c)
{
    int sockfd;
    if (server->client_sock > -1)
        sockfd = server->client_sock;
    else
        sockfd = server->openSocket();
        string command = "MOVP " + to_string((int)c.x) + " " + to_string((int)c.y) + " " + to_string((int)c.z) + " # # #";
        const char* movt = command.c_str();
        server->sendCommand(movt, sockfd);
}

void Robot::rotateGripper(double angle)
{
    this->resetJoints();

    int sockfd;
    if (server->client_sock > -1)
        sockfd = server->client_sock;
    else
        sockfd = server->openSocket();

    string command = "MOVJ # # # # # " + to_string(angle);
    const char* grip = command.c_str();
    server->sendCommand(grip, sockfd);
}

void Robot::resetJoints()
{
    int sockfd;
    if (server->client_sock > -1)
        sockfd = server->client_sock;
    else
        sockfd = server->openSocket();

    string command = "MOVJ 0 45 -45 0 -90 90";
    const char* grip = command.c_str();
    server->sendCommand(grip, sockfd);
}

void Robot::goHome()
{
    int sockfd;
    if (server->client_sock > -1)
        sockfd = server->client_sock;
    else
        sockfd = server->openSocket();

    string command = "GOHOME";
    const char* movt = command.c_str();
    server->sendCommand(movt, sockfd);
}

void Robot::dropPiece(int col)
{
    float x = board_x0 + col * board_dx;
    float y = board_y;
    float z = board_z_hover;

    // hover
    RobotCoord position_hover = {x, y, z};
    move(position_hover);

    // drop
    setSpeed(30, 2);
    RobotCoord position_drop = {x, y, board_z_drop};
    move(position_drop);
    openGripper();
}


void Robot::openGripper()
{
    int sockfd;
    if (server->client_sock > -1)
        sockfd = server->client_sock;
    else
        sockfd = server->openSocket();

    string command = "OUTPUT 48 OFF";
    const char* open_grip = command.c_str();
    server->sendCommand(open_grip, sockfd);
}

void Robot::closeGripper()
{
    int sockfd;
    if (server->client_sock > -1)
        sockfd = server->client_sock;
    else
        sockfd = server->openSocket();

    string command = "OUTPUT 48 ON";
    const char* close_grip = command.c_str();
    server->sendCommand(close_grip, sockfd);
}

void Robot::sleep(int msec)
{
    int sockfd;
    if (server->client_sock > -1)
        sockfd = server->client_sock;
    else
        sockfd = server->openSocket();

    string command = "SLEEP " + to_string(msec);
    const char* sleep = command.c_str();
    server->sendCommand(sleep, sockfd);
}
