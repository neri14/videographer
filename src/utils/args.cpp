#include "args.h"
#include <utility>

#include <cstring>

#include <iostream>

namespace vgraph {
namespace utils {

args args::parse(int argc, char* argv[])
{
    args a;

    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "-h") == 0 || std::strcmp(argv[i], "--help") == 0)
            a.help = true;
        else if (std::strcmp(argv[i], "-d") == 0 || std::strcmp(argv[i], "--debug") == 0)
            a.debug = true;
    }

    return std::move(a);
}

void args::print_help()
{
    std::cout << "Usage: vgraph [flags]" << std::endl;
    std::cout << "Flags:" << std::endl;
    std::cout << "  -h, --help    Print this help message" << std::endl;
    std::cout << "  -d, --debug   Enable debug logs" << std::endl;
}

} // namespace utils
} // namespace vgraph
