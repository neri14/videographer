#include "overlay.h"

#include "string_widget.h"

#include <format>
#include <chrono>

namespace vgraph {
namespace video {
namespace overlay {

overlay::overlay()
{
    //TODO here will normally create widgets based on template file
    widgets_.push_back(std::make_shared<string_widget>(100, 100, "Hello World", "DejaVu Sans 64", color::white, color::black, 4, ETextAlign::Left));
}

overlay::~overlay()
{
    double total_s = total_drawing_time/1000000.0;

    log.info("Total drawing time: {:.6f} s", total_s);
    log.info("Average drawing time: {:.6f} s/frame", total_s/total_drawn_frames);
}

// std::string timestamp_str(double timestamp)
// {
//     double s = timestamp;
//     int m = static_cast<int>(s)/60;
//     int h = m/60;

//     m = m-h*60;
//     s = s-m*60;

//     return std::format("{:02d}:{:02d}:{:09.06f}",h,m,s);
// }

void overlay::draw(cairo_t* cr, double timestamp)
{
    auto t1 = std::chrono::high_resolution_clock::now();

    for (auto w : widgets_) {
        w->draw(cr);
    }

    total_drawing_time += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t1).count();
    total_drawn_frames++;
}

} // namespace overlay
} // namespace video
} // namespace vgraph
