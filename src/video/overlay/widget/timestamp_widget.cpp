#include "timestamp_widget.h"

extern "C" {
    #include <pango/pangocairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

timestamp_widget::timestamp_widget(int x,
                                   int y,
                                   ETextAlign align,
                                   const std::string& font,
                                   const rgba& color,
                                   const rgba& border_color,
                                   int border_width,
                                   const std::string& format,
                                   int utcoffset) :
    text_widget::text_widget(EType_Dynamic, x, y, align, font, color, border_color, border_width),
    format_("{:" + format + "}"),
    utcoffset_(utcoffset)
{
    log.debug("Created timestamp widget");
}

timestamp_widget::~timestamp_widget()
{}

void timestamp_widget::draw_dynamic_impl(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data)
{
    auto tpzoned = std::chrono::time_point_cast<std::chrono::seconds>(data->timestamp + std::chrono::minutes(utcoffset_));
    draw_text(cr, std::vformat(format_, std::make_format_args(tpzoned)));
}

} // namespace overlay
} // namespace video
} // namespace vgraph
