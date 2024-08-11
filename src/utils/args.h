#ifndef ARGS_H
#define ARGS_H

namespace vgraph {
namespace utils {

struct args {
    bool debug;

    static args parse(int argc, char* argv[]);
};

} // namespace utils
} // namespace vgraph

#endif // ARGS_H