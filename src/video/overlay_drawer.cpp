#include "overlay_drawer.h"
#include <format>

namespace vgraph {
namespace video {

void overlay_drawer::draw(cairo_t* cr, double timestamp) const
{
    double s = timestamp;
    int m = static_cast<int>(s)/60;
    int h = m/60;

    m = m-h*60;
    s = s-m*60;

    cairo_scale (cr, 4, 4);
    cairo_move_to(cr, 250, 25);
    cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 1);
    cairo_show_text(cr, std::format("{:02d}:{:02d}:{:09.06f}",h,m,s).c_str());
}

} // namespace video
} // namespace vgraph
