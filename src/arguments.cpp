#include "arguments.h"
#include "utils/argument_parser.h"

namespace vgraph {
namespace key {
    std::string help("help");
    std::string debug("debug");
    std::string gpu("gpu");
    std::string telemetry("telemetry");
    std::string input("input");
    std::string output("output");
    std::string resolution("resolution");
    std::string bitrate("bitrate");
}

utils::argument_parser prepare_parser()
{
    utils::argument_parser parser("vgraph");

    parser.add_argument(key::help, utils::argument().flag().option("-h").option("--help").description("Print this help message"));
    parser.add_argument(key::debug, utils::argument().flag().option("-d").option("--debug").description("Enable debug logs"));
    parser.add_argument(key::gpu, utils::argument().flag().option("-g").option("--gpu").description("Use Nvidia GPU for processing"));

    parser.add_argument(key::telemetry, utils::argument().option("-t").option("--telemetry").description("Telemetry file path"));
    parser.add_argument(key::input, utils::argument().option("-i").option("--input").description("Input video file path"));
    parser.add_argument(key::output, utils::argument().option("-o").option("--output").description("Output video file path"));
    
    parser.add_argument(key::resolution, utils::argument().option("-r").option("--resolution").description("Output video resolution, format: WIDTHxHEIGHT"));
    parser.add_argument(key::bitrate, utils::argument().option("-b").option("--bitrate").description("Output video bitrate, in kbit/s"));

    return parser;
}

template <typename T>
bool read_mandatory_value(const utils::argument_parser& parser, const std::string& name, utils::logging::logger& log, T& value_out)
{
    if (!parser.has(name)) {
        log.error("Missing mandatory {} argument", name);
        return false;
    }

    if (parser.count(name) > 1) {
        log.error("Expected 1 value for {} got {}", name, parser.count(name));
        return false;
    }

    value_out = parser.get<T>(name);
    return true;
}

bool parse_resolution(const std::string& str, utils::logging::logger& log, std::pair<int, int>& out)
{
    try {
        int delimiter_pos = str.find('x');
        out.first = std::stoi(str.substr(0, delimiter_pos));
        out.second = std::stoi(str.substr(delimiter_pos+1, std::string::npos));
    } catch (...) {
        log.error("Error parsing \"{}\" as resolution, expected format is \"WIDTHxHEIGHT\"", str);
        return false;
    }
    return true;
}

arguments read_args(const utils::argument_parser& parser, utils::logging::logger& log)
{
    arguments a;
    bool valid = true;

    a.debug = parser.get<bool>(key::debug);
    a.gpu = parser.get<bool>(key::gpu);
    
    valid = read_mandatory_value<std::string>(parser, key::telemetry, log, a.telemetry) && valid;
    valid = read_mandatory_value<std::string>(parser, key::input, log, a.input) && valid;
    valid = read_mandatory_value<std::string>(parser, key::output, log, a.output) && valid;

    std::string res_str;
    if (read_mandatory_value<std::string>(parser, key::resolution, log, res_str)) {
        valid = parse_resolution(res_str, log, a.resolution) && valid;
    } else {
        valid = false;
    }

    valid = read_mandatory_value<int>(parser, key::bitrate, log, a.bitrate) && valid;

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
