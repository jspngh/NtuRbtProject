#include <iostream>

#include "board.hpp"

using namespace std;

int main (int argc, char* args[])
{
    Board b;
    //b.doMove(1, 1);
    //b.doMove(1, 1);
    //b.doMove(1, 1);
    b.doMove(1, 1);

    b.doMove(3, 0);
    b.doMove(4, 0);
    b.doMove(2, 0);
    b.doMove(5, 0);

    b.doMove(2, 1);
    b.doMove(3, 1);
    b.doMove(3, 1);
    b.doMove(4, 1);
    b.doMove(4, 1);
    b.doMove(4, 1);

    cout << b;

    int score = b.countStreaks(1,4);
    int score0 = b.countStreaks(0, 4);
    cout << "score:" << endl;
    cout << score << endl;
    cout << "score:" << endl;
    cout << score0 << endl;
}
