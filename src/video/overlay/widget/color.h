#ifndef RGBA_H
#define RGBA_H

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
}

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // RGBA_H
