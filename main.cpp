#include <iostream>

#include "board.hpp"

using namespace std;

int main (int argc, char* args[])
{
    Board b;
    cout << b;
    cout << b.doMove(1,1) << endl;;
    cout << b.doMove(1,1) << endl;;
    cout << b.doMove(1,1) << endl;;
    cout << b.doMove(1,1) << endl;;
    cout << b.doMove(1,1) << endl;;
    cout << b.doMove(1,1) << endl;;
    cout << b.doMove(1,1) << endl;;
    cout << b.doMove(2,0) << endl;;
    cout << b.doMove(1,0) << endl;;
    cout << b;
}
