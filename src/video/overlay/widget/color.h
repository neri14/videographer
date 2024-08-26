#ifndef RGBA_H
#define RGBA_H

#include <string>
#include <map>

namespace vgraph {
namespace video {
namespace overlay {

struct rgba {
    double r;
    double g;
    double b;
    double a = 1.0;
};
using rgb = rgba;

namespace color {
    const rgb white{1.0, 1.0, 1.0};
    const rgb black{0.0, 0.0, 0.0};

    const std::map<std::string, rgba> map = {
        {"white", color::white},
        {"black", color::black}
    };
}



rgba color_from_string(const std::string& str);

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // RGBA_H
