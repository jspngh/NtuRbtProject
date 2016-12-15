#include <iostream>

#include "board.hpp"

using namespace std;

int main (int argc, char* args[])
{
    Board b;
    cout << b;
    b.doMove(1,1);
    b.doMove(1,1);
    b.doMove(1,1);
    b.doMove(1,1);
    b.doMove(1,1);
    b.doMove(1,1);
    b.doMove(1,1);
    b.doMove(2,0);
    b.doMove(1,0);
    cout << b;

    cout << b.countSteaks(0, 4) << endl;
    cout << b.countSteaks(1, 4) << endl;
}
