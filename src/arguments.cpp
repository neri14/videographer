#include "arguments.h"
#include "utils/argument_parser.h"

namespace vgraph {
namespace key {
    std::string help("help");
    std::string debug("debug");
    std::string telemetry("telemetry");
}

utils::argument_parser prepare_parser()
{
    utils::argument_parser parser("vgraph");

    parser.add_argument(key::help, utils::argument().flag().option("-h").option("--help").description("Print this help message"));
    parser.add_argument(key::debug, utils::argument().flag().option("-d").option("--debug").description("Enable debug logs"));
    parser.add_argument(key::telemetry, utils::argument().option("-t").option("--telemetry").description("Telemetry file path"));

    return parser;
}

arguments read_args(utils::argument_parser parser, utils::logging::logger& log) {
    arguments a;

    a.debug = parser.get<bool>(key::debug);

    if (parser.count(key::telemetry) != 1) {
        log.error("Expected 1 telemetry file to be provided, got {}", parser.count(key::telemetry));
        exit(1);
    }
    a.telemetry = parser.get<std::string>(key::telemetry);

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
