#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string>
#include <optional>

namespace vgraph {

struct arguments {
    bool debug = false;
    bool gpu = false;

    std::optional<std::string> telemetry = std::nullopt;
    std::optional<std::string> layout = std::nullopt;
    std::string input = "";
    std::string output = "";

    std::optional<double> offset = std::nullopt;
    bool timecode = false;

    std::pair<int, int> resolution = {3840,2160};
    int bitrate = {80*1024};

    static arguments parse(int argc, char* argv[]);
};

} // namespace vgraph

#endif // ARGUMENTS_H
