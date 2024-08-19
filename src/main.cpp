#include "utils/argument_parser.h"
#include "utils/logging/stream_sink.h"
#include "utils/logging/backend.h"

#include "telemetry/parser.h"

#include <iostream>

namespace arg {
    const std::string help("help");
    const std::string debug("debug");
    const std::string telemetry("telemetry");
}

vgraph::utils::argument_parser parse_args(int argc, char* argv[])
{
    vgraph::utils::logging::logger log{"parse_args()"};

    vgraph::utils::argument_parser parser("vgraph");
    try {
        using vgraph::utils::argument;
        parser.add_argument(arg::help, argument().option("-h").option("--help").flag().description("Print this help message"));
        parser.add_argument(arg::debug, argument().option("-d").option("--debug").flag().description("Enable debug logs"));
        parser.add_argument(arg::telemetry, argument().option("-t").option("--telemetry").description("Telemetry file path"));
        parser.parse(argc, argv);
    } catch (vgraph::utils::argument_exception e) {
        log.error(e.what());
        exit(1);
    } catch (std::exception e) {
        log.error("Unexpected error: {}", e.what());
        exit(1);
    }

    return std::move(parser);
}

void enable_logging(bool debug = true)
{
    using namespace vgraph::utils;
    static std::shared_ptr<logging::stream_sink> stdout_sink = std::make_shared<logging::stream_sink>(std::cout);

    auto& backend = logging::backend::get_instance();

    backend.remove_sink(stdout_sink);
    backend.add_sink(debug ? logging::ELogLevel::Debug : logging::ELogLevel::Info, stdout_sink);
}

// std::shared_ptr<vgraph::telemetry::parser> init_telemetry(vgraph::utils::argument_parser args)
// {
//     vgraph::utils::logging::logger log{"init_telemetry()"};

//     std::shared_ptr<vgraph::telemetry::parser> telmetry = init_telemetry(args);
//     if (!args.has(arg::telemetry)) {
//         log.error("No telemetry file provided - exitting...");
//         exit(1);
//     }
//     if (args.count(arg::telemetry) > 1) {
//         log.error("Multiple telemetry files provided - exitting...");
//         exit(1);
//     }

//     return vgraph::telemetry::make_parser(args.get<std::string>(arg::telemetry));
// }

int main(int argc, char* argv[])
{
    enable_logging();

    auto args = parse_args(argc, argv);

    if (args.get<bool>(arg::help)) {
        args.print_help();
        return 0;
    }

    bool debug = args.get<bool>(arg::debug);
    enable_logging(debug);

    vgraph::utils::logging::logger log{"main()"};
    log.info("Log level set to {}", debug ? "DEBUG" : "INFO");

    // std::shared_ptr<vgraph::telemetry::parser> telmetry = init_telemetry(args);
    //TODO telemetry->parse()


    log.info("Here be dragons");
    // TODO to be implemented

    return 0;
}
