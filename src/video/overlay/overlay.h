#ifndef OVERLAY_H
#define OVERLAY_H

#include "utils/logging/logger.h"

extern "C" {
    #include <cairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

class overlay {
public:
    overlay() = default;
    ~overlay();

    void draw(cairo_t* cr, double timestamp);

private:
    utils::logging::logger log{"overlay"};

    long total_drawing_time = 0;
    int total_drawn_frames = 0;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // OVERLAY_H
