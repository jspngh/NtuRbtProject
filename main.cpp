#include <iostream>

#include "board.hpp"

using namespace std;

int main (int argc, char* args[])
{
    Board* b = new Board();
    cout << *b;
    delete b;
}
