#include "rectangle_widget.h"

extern "C" {
    #include <pango/pangocairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {
rectangle_widget::rectangle_widget(int x, int y, int width, int height, const rgba& bg_color, const rgba& border_color, int border_width) :
    widget(EType_Static), x_(x), y_(y), width_(width), height_(height), bg_color_(bg_color), border_color_(border_color), border_width_(border_width)
{
    log.debug("Created rectangle widget at ({},{}) and size ({},{})", x, y, width, height);
}

rectangle_widget::~rectangle_widget()
{}

void rectangle_widget::draw_static_impl(cairo_t* cr)
{
    cairo_rectangle(cr, x_, y_, width_, height_);

    cairo_set_line_width(cr, border_width_);
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);

    cairo_set_source_rgba(cr, bg_color_.r, bg_color_.g, bg_color_.b, bg_color_.a);
    cairo_fill_preserve(cr);

    cairo_set_source_rgba(cr, border_color_.r, border_color_.g, border_color_.b, border_color_.a);
    cairo_stroke(cr);
}

} // namespace overlay
} // namespace video
} // namespace vgraph
