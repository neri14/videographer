#include "string_widget.h"

extern "C" {
    #include <pango/pangocairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {
string_widget::string_widget(int x,
                             int y,
                             ETextAlign align,
                             const std::string& font,
                             const rgba& color,
                             const rgba& border_color,
                             int border_width,
                             const std::string& str) :
    text_widget::text_widget(EType_Static, x, y, align, font, color, border_color, border_width),
    str_(str)
{
    log.debug("Created string widget with text: \"{}\"", str_);
}

string_widget::~string_widget()
{}

void string_widget::draw_static_impl(cairo_t* cr)
{
    draw_text(cr, str_);
}

} // namespace overlay
} // namespace video
} // namespace vgraph
