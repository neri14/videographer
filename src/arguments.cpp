#include "arguments.h"
#include "utils/argument_parser.h"

namespace vgraph {
namespace key {
    std::string help("help");
    std::string debug("debug");
    std::string gpu("gpu");
    std::string telemetry("telemetry");
    std::string layout("layout");
    std::string offset("offset");
    std::string input("input");
    std::string output("output");
    std::string timecode("timecode");
    std::string resolution("resolution");
    std::string bitrate("bitrate");
}

/* helpers forward declarations */
utils::argument_parser prepare_parser();

arguments read_args(const utils::argument_parser& parser, utils::logging::logger& log);

template <typename T>
bool read_value(const utils::argument_parser& parser, const std::string& name, utils::logging::logger& log, std::optional<T>& value_out);

bool parse_resolution(const std::string& str, utils::logging::logger& log, std::optional<std::pair<int, int>>& out);

/* interface */
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

/* helpers */
utils::argument_parser prepare_parser()
{
    utils::argument_parser parser("vgraph");

    parser.add_argument(key::help,       utils::argument().flag().option("-h").option("--help")      .description("Print this help message"));
    parser.add_argument(key::debug,      utils::argument().flag().option("-d").option("--debug")     .description("Enable debug logs"));
    parser.add_argument(key::gpu,        utils::argument().flag().option("-g").option("--gpu")       .description("Use Nvidia GPU for processing"));
    parser.add_argument(key::timecode,   utils::argument().flag().option("-c").option("--timecode")  .description("Draw a timecode on each frame"));
    parser.add_argument(key::telemetry,  utils::argument()       .option("-t").option("--telemetry") .description("Telemetry file path"));
    parser.add_argument(key::layout,     utils::argument()       .option("-a").option("--layout")    .description("Layout file path"));
    parser.add_argument(key::input,      utils::argument()       .option("-i").option("--input")     .description("Input video file path"));
    parser.add_argument(key::output,     utils::argument()       .option("-o").option("--output")    .description("Output video file path"));
    parser.add_argument(key::offset,     utils::argument()       .option("-s").option("--offset")    .description("Telemetry offset in seconds (video time at which track starts)"));
    parser.add_argument(key::resolution, utils::argument()       .option("-r").option("--resolution").description("Output video resolution, format: WIDTHxHEIGHT"));
    parser.add_argument(key::bitrate,    utils::argument()       .option("-b").option("--bitrate")   .description("Output video bitrate, in kbit/s"));

    return parser;
}

arguments read_args(const utils::argument_parser& parser, utils::logging::logger& log)
{
    arguments a;
    bool valid = true;

    a.debug = parser.get<bool>(key::debug);
    a.gpu = parser.get<bool>(key::gpu);
    a.timecode = parser.get<bool>(key::gpu);

    valid = read_value<std::string>(parser, key::telemetry, log, a.telemetry) && valid;
    valid = read_value<std::string>(parser, key::layout, log, a.layout) && valid;
    valid = read_value<std::string>(parser, key::input, log, a.input) && valid;
    valid = read_value<std::string>(parser, key::output, log, a.output) && valid;
    valid = read_value<double>(parser, key::offset, log, a.offset) && valid;
    valid = read_value<int>(parser, key::bitrate, log, a.bitrate) && valid;
    
    std::optional<std::string> res_str;
    if (read_value<std::string>(parser, key::resolution, log, res_str) && res_str) {
        valid = parse_resolution(*res_str, log, a.resolution) && valid;
    }

    if (!valid) {
        exit(1);
    }

    return std::move(a);
}

void assert_argument_set_valid(const arguments& args, utils::logging::logger& log)
{
    bool valid = true;

    if (!args.input) {
        log.error("Missing mandatory argument: input");
        valid = false;
    }
    if (!args.output) {
        log.error("Missing mandatory argument: output");
        valid = false;
    }
    if (!args.resolution) {
        log.error("Missing mandatory argument: resolution");
        valid = false;
    }
    if (!args.bitrate) {
        log.error("Missing mandatory argument: bitrate");
        valid = false;
    }

    if (!false) {
        exit(1);
    }
}

template <typename T>
bool read_value(const utils::argument_parser& parser, const std::string& name, utils::logging::logger& log, std::optional<T>& value_out)
{
    if (!parser.has(name)) {
        return true;
    }

    if (parser.count(name) > 1) {
        log.error("Expected 1 value for {} got {}", name, parser.count(name));
        return false;
    }

    value_out = parser.get<T>(name);
    return true;
}

bool parse_resolution(const std::string& str, utils::logging::logger& log, std::optional<std::pair<int, int>>& out)
{
    try {
        int delimiter_pos = str.find('x');
        out = std::make_pair(std::stoi(str.substr(0, delimiter_pos)),  std::stoi(str.substr(delimiter_pos+1, std::string::npos)));
        log.debug("Parsed resolution width={} height={} ", out->first, out->second);
    } catch (...) {
        log.error("Error parsing \"{}\" as resolution, expected format is \"WIDTHxHEIGHT\"", str);
        return false;
    }
    return true;
}



}
