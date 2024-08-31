#include "line_widget.h"

extern "C" {
    #include <pango/pangocairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

line_widget::line_widget(int x1, int y1, int x2, int y2, const rgba& color, int width) :
    widget(EType_Static), x1_(x1), y1_(y1), x2_(x2), y2_(y2), color_(color), width_(width)
{
    log.debug("Created line widget from ({},{}) to ({},{})", x1, y1, x2, y2);
}

line_widget::~line_widget()
{}

void line_widget::draw_static_impl(cairo_t* cr)
{
    cairo_move_to(cr, x1_, y1_);
    cairo_line_to(cr, x2_, y2_);

    cairo_set_line_width(cr, width_);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);

    cairo_set_source_rgba(cr, color_.r, color_.g, color_.b, color_.a);
    cairo_stroke(cr);
}

} // namespace overlay
} // namespace video
} // namespace vgraph
