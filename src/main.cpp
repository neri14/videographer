#include <iostream>

#include "utils/argument_parser.h"

int main(int argc, char* argv[])
{
    vgraph::utils::argument_parser parser("vgraph");

    {
        using vgraph::utils::argument;
        parser.add_argument("help", argument().option("-h").option("--help").flag().description("Print this help message"));
        parser.add_argument("debug", argument().option("-d").option("--debug").flag().description("Enable debug logs"));
        parser.add_argument("telemetry", argument().option("-t").option("--telemetry").description("Telemetry file path"));
    }

    parser.parse(argc, argv);

    if (parser.get<bool>("help")) {
        parser.print_help();
        return 0;
    }

    return 0;
}
