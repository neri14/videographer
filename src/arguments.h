#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string>
#include <optional>

namespace vgraph {

struct arguments {
    bool debug = false;
    bool gpu = false;
    bool timecode = false;
    bool alignment_mode = false;

    std::optional<std::string> telemetry = std::nullopt;
    std::optional<std::string> layout = std::nullopt;
    std::optional<std::string> input = std::nullopt;
    std::optional<std::string> output = std::nullopt;

    std::optional<double> offset = std::nullopt;

    std::optional<std::pair<int, int>> resolution = std::nullopt;
    std::optional<int> bitrate = std::nullopt;

    std::optional<double> clip_time = std::nullopt;

    static arguments parse(int argc, char* argv[]);
};

} // namespace vgraph

#endif // ARGUMENTS_H
