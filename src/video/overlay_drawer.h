#ifndef OVERLAY_H
#define OVERLAY_H

#include "utils/logging/logger.h"

extern "C" {
    #include <cairo.h>
}

namespace vgraph {
namespace video {

class overlay_drawer {
public:
    overlay_drawer() = default;
    ~overlay_drawer() = default;

    void draw(cairo_t* cr, double timestamp) const;

private:
    utils::logging::logger log{"overlay"};
};

} // namespace video
} // namespace vgraph

#endif // OVERLAY_H
