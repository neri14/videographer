#include "arguments.h"
#include "utils/argument_parser.h"

#include <fstream>
#include <sstream>

namespace vgraph {
namespace consts {
    const int defult_alignment_clip_time(60);
    const std::string whitespaces(" \t\v\r\n");
}
namespace key {
    std::string help("help");
    std::string version("version");
    std::string config("config");
    std::string debug("debug");
    std::string gpu("gpu");
    std::string timecode("timecode");
    std::string alignment("alignment");
    std::string telemetry("telemetry");
    std::string layout("layout");
    std::string offset("offset");
    std::string input("input");
    std::string output("output");
    std::string resolution("resolution");
    std::string bitrate("bitrate");
    std::string clip_time("clip_time");
}

/* helpers forward declarations */
utils::argument_parser prepare_parser();

arguments read_args(const utils::argument_parser& parser, utils::logging::logger& log);

bool parse_config_file(const std::string& path, arguments& args);

void assert_arguments_valid(const arguments& args, utils::logging::logger& log);

template <typename T>
bool read_value(const utils::argument_parser& parser, const std::string& name, utils::logging::logger& log, std::optional<T>& value_out);

bool parse_resolution(const std::string& str, utils::logging::logger& log, std::optional<std::pair<int, int>>& out);

/* specialized parse_value functions */
template <typename T>
bool parse_value(const std::string& str, utils::logging::logger& log, T& value_out)
{
    log.error("Unsupported value type");
    return false;
}

template <>
bool parse_value<bool>(const std::string& str, utils::logging::logger& log, bool& value_out)
{
    if (str == "true") {
        value_out = true;
        return true;
    } else if (str == "false") {
        value_out = false;
        return true;
    }

    return false;
}

template <>
bool parse_value<std::optional<std::string>>(const std::string& str, utils::logging::logger& log, std::optional<std::string>& value_out)
{
    value_out = str;
    return true;
}

template <>
bool parse_value<std::optional<int>>(const std::string& str, utils::logging::logger& log, std::optional<int>& value_out)
{
    try {
        value_out = std::stoi(str);
    } catch (std::invalid_argument) {
        log.error("Could not parse value '{}' as integer point number", str);
        return false;
    } catch (std::out_of_range) {
        log.error("Value '{}' is out of range", str);
        return false;
    }

    return true;
}

template <>
bool parse_value<std::optional<double>>(const std::string& str, utils::logging::logger& log, std::optional<double>& value_out)
{
    try {
        value_out = std::stod(str);
    } catch (std::invalid_argument) {
        log.error("Could not parse value '{}' as floating point number", str);
        return false;
    } catch (std::out_of_range) {
        log.error("Value '{}' is out of range", str);
        return false;
    }

    return true;
}

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

        if (parser.get<bool>(key::version)) {
            parser.print_version();
            exit(0); //ok case
        }

        auto args =read_args(parser, log);
        assert_arguments_valid(args, log);
        return std::move(args);

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
    parser.add_argument(key::version,    utils::argument().flag()             .option("--version")   .description("Print version"));
    parser.add_argument(key::config,     utils::argument()       .option("-c").option("--config")    .description("Load config file"));
    parser.add_argument(key::debug,      utils::argument().flag().option("-d").option("--debug")     .description("Enable debug logs"));
    parser.add_argument(key::gpu,        utils::argument().flag().option("-g").option("--gpu")       .description("Use Nvidia GPU for processing"));
    parser.add_argument(key::timecode,   utils::argument().flag().option("-m").option("--timecode")  .description("Draw a timecode on each frame"));
    parser.add_argument(key::alignment,  utils::argument().flag().option("-l").option("--alignment") .description(std::format("Enable alignment mode ({}s clip unless overriden by -T)", consts::defult_alignment_clip_time)));
    parser.add_argument(key::telemetry,  utils::argument()       .option("-t").option("--telemetry") .description("Telemetry file path"));
    parser.add_argument(key::layout,     utils::argument()       .option("-a").option("--layout")    .description("Layout file path"));
    parser.add_argument(key::input,      utils::argument()       .option("-i").option("--input")     .description("Input video file path"));
    parser.add_argument(key::output,     utils::argument()       .option("-o").option("--output")    .description("Output video file path"));
    parser.add_argument(key::offset,     utils::argument()       .option("-s").option("--offset")    .description("Telemetry offset in seconds (video time at which track starts)"));
    parser.add_argument(key::resolution, utils::argument()       .option("-r").option("--resolution").description("Output video resolution, format: WIDTHxHEIGHT"));
    parser.add_argument(key::bitrate,    utils::argument()       .option("-b").option("--bitrate")   .description("Output video bitrate, in kbit/s"));
    parser.add_argument(key::clip_time,  utils::argument()       .option("-T").option("--clip-time") .description("Generate clip limited to provided time in seconds (e.g. in combination with alignment mode)"));

    return parser;
}

arguments read_args(const utils::argument_parser& parser, utils::logging::logger& log)
{
    arguments a;
    bool valid = true;

    if (parser.has(key::config)) {
        valid = parse_config_file(parser.get<std::string>(key::config), a);
    }

    a.debug = a.debug || parser.get<bool>(key::debug);
    a.gpu = a.gpu || parser.get<bool>(key::gpu);
    a.timecode = a.timecode || parser.get<bool>(key::timecode);
    a.alignment_mode = a.alignment_mode || parser.get<bool>(key::alignment);

    valid = read_value<std::string>(parser, key::telemetry, log, a.telemetry) && valid;
    valid = read_value<std::string>(parser, key::layout, log, a.layout) && valid;
    valid = read_value<std::string>(parser, key::input, log, a.input) && valid;
    valid = read_value<std::string>(parser, key::output, log, a.output) && valid;
    valid = read_value<double>(parser, key::offset, log, a.offset) && valid;
    valid = read_value<int>(parser, key::bitrate, log, a.bitrate) && valid;
    valid = read_value<double>(parser, key::clip_time, log, a.clip_time) && valid;
    
    std::optional<std::string> res_str;
    if (read_value<std::string>(parser, key::resolution, log, res_str) && res_str) {
        valid = parse_resolution(*res_str, log, a.resolution) && valid;
    }

    if (a.alignment_mode && !a.clip_time) {
        a.clip_time = consts::defult_alignment_clip_time;
    }

    if (!valid) {
        exit(1);
    }

    return std::move(a);
}

/* Expected format:
#comments are ignored
key=value
key=value
key=value
 */
bool parse_config_file(const std::string& path, arguments& args)
{
    utils::logging::logger log{"arguments::parse_config_file()"};

    std::ifstream file(path);
    if (!file.is_open()) {
        log.error("Failed to open '{}' file", path);
        return false;
    }

    std::string line;
    while(getline(file, line)) {
        line = line.substr(line.find_first_not_of(consts::whitespaces)); //remove leading whitespaces
        if (line.starts_with('#')) {
            continue;
        }

        int pos = line.find('=');
        const std::string& key = line.substr(0, pos);
        const std::string& value = line.substr(pos+1);
        
        log.debug("key: '{}' value: '{}'", key, value);
        if (key == key::debug) {
            parse_value(value, log, args.debug);
        } else if (key == key::gpu) {
            parse_value(value, log, args.gpu);
        } else if (key == key::timecode) {
            parse_value(value, log, args.timecode);
        } else if (key == key::alignment) {
            parse_value(value, log, args.alignment_mode);
        } else if (key == key::telemetry) {
            parse_value(value, log, args.telemetry);
        } else if (key == key::layout) {
            parse_value(value, log, args.layout);
        } else if (key == key::input) {
            parse_value(value, log, args.input);
        } else if (key == key::output) {
            parse_value(value, log, args.output);
        } else if (key == key::offset) {
            parse_value(value, log, args.offset);
        } else if (key == key::resolution) {
            parse_resolution(value, log, args.resolution);
        } else if (key == key::bitrate) {
            parse_value(value, log, args.bitrate);
        } else if (key == key::clip_time) {
            parse_value(value, log, args.clip_time);
        }
    }

    return true;
}

void assert_arguments_valid(const arguments& args, utils::logging::logger& log)
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
    if (args.alignment_mode && args.offset) {
        log.error("Offset not allowed to be set when in alignment mode");
        valid = false;
    }

    if (!valid) {
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
