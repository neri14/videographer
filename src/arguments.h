#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string>
#include <vector>

namespace vgraph {

struct arguments {
    bool debug = false;
    std::string telemetry = "";
    std::vector<std::string> input = {};
    std::string output = "";

    static arguments parse(int argc, char* argv[]);
};

} // namespace vgraph

#endif // ARGUMENTS_H
