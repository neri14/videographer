#ifndef GENERATOR_H
#define GENERATOR_H

#include "utils/logging/logger.h"

#include <string>
#include <vector>

namespace vgraph {
namespace video {

class generator {
public:
    generator(const std::vector<std::string>& input, const std::string& output);
    ~generator() = default;

    void generate();

private:
    utils::logging::logger log{"generator"};

    std::vector<std::string> input_;
    std::string output_;
};

}
}

#endif // GENERATOR_H