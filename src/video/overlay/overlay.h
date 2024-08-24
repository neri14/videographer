#ifndef OVERLAY_H
#define OVERLAY_H

#include "utils/logging/logger.h"
#include "widget.h"
#include <memory>

extern "C" {
    #include <cairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

class overlay {
public:
    overlay();
    ~overlay();

    void draw(cairo_t* cr, double timestamp);

private:
    utils::logging::logger log{"overlay"};

    long total_drawing_time = 0;
    int total_drawn_frames = 0;

    std::vector<std::shared_ptr<widget>> widgets_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // OVERLAY_H
