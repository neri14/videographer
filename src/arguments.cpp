#include "arguments.h"
#include "utils/argument_parser.h"

namespace vgraph {
namespace key {
    std::string help("help");
    std::string debug("debug");
    std::string telemetry("telemetry");
    std::string input("input");
    std::string output("output");
}

utils::argument_parser prepare_parser()
{
    utils::argument_parser parser("vgraph");

    parser.add_argument(key::help, utils::argument().flag().option("-h").option("--help").description("Print this help message"));
    parser.add_argument(key::debug, utils::argument().flag().option("-d").option("--debug").description("Enable debug logs"));
    parser.add_argument(key::telemetry, utils::argument().option("-t").option("--telemetry").description("Telemetry file path"));
    parser.add_argument(key::input, utils::argument().option("-i").option("--input").description("Input video file(s) path"));
    parser.add_argument(key::output, utils::argument().option("-o").option("--output").description("Output video file path"));

    return parser;
}

arguments read_args(utils::argument_parser parser, utils::logging::logger& log)
{
    arguments a;
    bool valid = true;

    a.debug = parser.get<bool>(key::debug);

    if (parser.count(key::telemetry) != 1) {
        log.error("Expected one telemetry file to be provided, got {}", parser.count(key::telemetry));
        valid = false;
    } else {
        a.telemetry = parser.get<std::string>(key::telemetry);
    }

    if (parser.has(key::input)) {
        a.input = parser.get<std::vector<std::string>>(key::input);
    } //no input file is valid configuration for overlay only mode

    if (parser.count(key::output) != 1) {
        log.error("Expected one output file path to be provided, got {}", parser.count(key::output));
        valid = false;
    } else {
        a.output = parser.get<std::string>(key::output);
    }

    if (!valid) {
        exit(1);
    }

    return std::move(a);
}

arguments arguments::parse(int argc, char* argv[])
{
    utils::logging::logger log{"arguments::parse()"};

    try {
        auto parser = prepare_parser();

        parser.parse(argc, argv);
        if (parser.get<bool>(key::help)) {
            parser.print_help();
            exit(0); //ok case
        }

        return std::move(read_args(parser, log));

    } catch (utils::argument_exception e) {
        log.error(e.what());
        exit(1);
    } catch (std::exception e) {
        log.error("Unexpected error: {}", e.what());
        exit(1);
    }

    //shall never reach
    log.error("Execution error");
    exit(2);
}

}
