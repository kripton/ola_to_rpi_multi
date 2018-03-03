#include <iostream>

#include "OlaToRpiMulti.hpp"

using namespace std;

int main()
{
    OlaToRpiMulti otrm;

    otrm.Init();

    otrm.Start();


    while(1)
    {
        usleep(5000);
    }

    return 0;
}
