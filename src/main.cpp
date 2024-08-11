#include <iostream>

#include "utils/args.h"

int main(int argc, char* argv[])
{
    vgraph::utils::args a = vgraph::utils::args::parse(argc, argv);

    if (a.help) {
        vgraph::utils::args::print_help();
        return 0;
    }

    if (a.debug)
        std::cout << "DEBUG" << std::endl;
    else
        std::cout << "NO DEBUG" << std::endl;

    return 0;
}
