#ifndef OVERLAY_H
#define OVERLAY_H

#include "utils/logging/logger.h"
#include "widget/widget.h"
#include <memory>

extern "C" {
    #include <cairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

class overlay {
public:
    overlay(std::pair<int, int> resolution);
    ~overlay();

    void precache();
    void draw(cairo_t* cr, double timestamp);

private:
    utils::logging::logger log{"overlay"};

    void update_dynamic_cache(double timestamp);
    void add_widget(std::shared_ptr<widget> ptr);

    int width;
    int height;

    long total_drawing_time = 0;
    int total_drawn_frames = 0;

    int cache_hit = 0;
    int cache_miss = 0;

    std::vector<std::shared_ptr<widget>> static_widgets_;
    std::vector<std::shared_ptr<widget>> dynamic_widgets_;

    cairo_surface_t* static_cache;
    cairo_surface_t* dynamic_cache;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // OVERLAY_H
