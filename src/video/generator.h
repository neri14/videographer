#ifndef GENERATOR_H
#define GENERATOR_H

#include "utils/logging/logger.h"

#include "overlay/overlay.h"

#include <string>
#include <vector>

namespace vgraph {
namespace video {

class generator {
public:
    generator(const std::string& input, const std::string& output, overlay::overlay& overlay);
    ~generator() = default;

    void generate();

private:
    utils::logging::logger log{"generator"};

    std::string input_path_;
    std::string output_path_;

    overlay::overlay& overlay_;
};

}
}

#endif // GENERATOR_H