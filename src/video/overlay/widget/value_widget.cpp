#include "value_widget.h"

extern "C" {
    #include <pango/pangocairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {
value_widget::value_widget(int x,
                           int y,
                           ETextAlign align,
                           const std::string& font,
                           const rgba& color,
                           const rgba& border_color,
                           int border_width,
                           const std::string& key,
                           int precision) :
    text_widget::text_widget(EType_Dynamic, x, y, align, font, color, border_color, border_width),
    field_(telemetry::map_key_to_field(key)),
    precision_(precision)
{
    log.debug("Created value widget for \"{}\" with precision: {}", key, precision);
}

value_widget::~value_widget()
{}

void value_widget::draw_dynamic_impl(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data)
{
    std::string str = "-";
    if (data->fields.contains(field_)) {
        str = std::vformat("{1:.{0}f}", std::make_format_args(precision_, data->fields.at(field_)));
    }
    draw_text(cr, str);
}

} // namespace overlay
} // namespace video
} // namespace vgraph
