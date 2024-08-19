#include <iostream>

// #include "utils/args.h"
#include "utils/argument_parser.h"

int main(int argc, char* argv[])
{
    vgraph::utils::argument_parser parser("vgraph");

    {
        using vgraph::utils::argument;
        parser.add_argument("help", argument().option("-h").option("--help").flag().description("Print this help message"));
        parser.add_argument("debug", argument().option("-d").option("--debug").flag().description("Enable debug logs"));
        parser.add_argument("telemetry", argument().option("-t").option("--telemetry").mandatory().description("Telemetry file path"));
    }

    parser.print_help();

    // vgraph::utils::args a = vgraph::utils::args::parse(argc, argv);

    // if (a.help) {
    //     vgraph::utils::args::print_help();
    //     return 0;
    // }

    // if (a.debug)
    //     std::cout << "DEBUG" << std::endl;
    // else
    //     std::cout << "NO DEBUG" << std::endl;

    return 0;
}
