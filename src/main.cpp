#include "manager.h"

#include <iostream>

int main(int argc, char* argv[])
{
    vgraph::manager m;
    m.init(argc, argv);
    m.run();
    return 0;
}
