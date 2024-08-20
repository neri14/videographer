#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <string>

namespace vgraph {

struct arguments {
    bool debug = false;
    std::string telemetry = "";

    static arguments parse(int argc, char* argv[]);
};

} // namespace vgraph

#endif // ARGUMENTS_H
