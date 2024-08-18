#ifndef ARGS_H
#define ARGS_H


namespace vgraph {
namespace utils {

struct args {
    bool help = false;
    bool debug = false;

    static args parse(int argc, char* argv[]);
    static void print_help();
};

} // namespace utils
} // namespace vgraph

#endif // ARGS_H
