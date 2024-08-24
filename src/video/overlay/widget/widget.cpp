#include "widget.h"

namespace vgraph {
namespace video {
namespace overlay {

widget::widget()
{}

void widget::draw(cairo_t* cr)
{
    cairo_save(cr);
    draw_impl(cr);
    cairo_restore(cr);
}

} // namespace overlay
} // namespace video
} // namespace vgraph
