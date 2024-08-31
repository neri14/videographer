#include "offset_widget.h"

namespace vgraph {
namespace video {
namespace overlay {

offset_widget::offset_widget(int x,
                           int y,
                           ETextAlign align,
                           const std::string& font,
                           const rgba& color,
                           const rgba& border_color,
                           int border_width,
                           double offset) :
    text_widget::text_widget(EType_Volatile, x, y, align, font, color, border_color, border_width),
    offset_(offset)
{
    log.debug("Created offset widget with \"{}\" s offset", offset);
}

offset_widget::~offset_widget()
{}

void offset_widget::draw_volatile_impl(cairo_t* cr, double value)
{
    draw_text(cr, std::format("{:10.6f}", value));
}

double offset_widget::get_volatile_value(double timestamp, const telemetry::timedatapoint&, const telemetry::timedatapoint&)
{
    return timestamp - offset_;
}

} // namespace overlay
} // namespace video
} // namespace vgraph
