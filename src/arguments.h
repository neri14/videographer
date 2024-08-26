#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string>

namespace vgraph {

struct arguments {
    bool debug = false;
    bool gpu = false;

    std::string telemetry = "";
    std::string input = "";
    std::string output = "";

    bool timecode = false;

    std::pair<int, int> resolution = {3840,2160};
    int bitrate = {80*1024};

    static arguments parse(int argc, char* argv[]);
};

} // namespace vgraph

#endif // ARGUMENTS_H
