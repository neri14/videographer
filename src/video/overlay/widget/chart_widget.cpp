#include "chart_widget.h"

// extern "C" {
//     #include <pango/pangocairo.h>
// }

namespace vgraph {
namespace video {
namespace overlay {

chart_widget::chart_widget(int x, int y, int width, int height,
                           const rgba& line_color, int line_width,
                           const rgba& point_color, int point_size,
                           const std::string& x_key, const std::string& y_key) :
    widget::widget(EType_Static|EType_Dynamic),
    x_(x),
    y_(y),
    width_(width),
    height_(height_),
    line_color_(line_color),
    line_width_(line_width),
    point_color_(point_color),
    point_size_(point_size),
    x_field_(telemetry::map_key_to_field(x_key)),
    y_field_(telemetry::map_key_to_field(y_key))
{
    log.debug("Created chart widget: x axis: {}, y axis: {}", x_key, y_key);
}

chart_widget::~chart_widget()
{}

void chart_widget::draw_static_impl(cairo_t* cr)
{
    log.warning("Not yet implemented: chart_widget::draw_static_impl");
}

void chart_widget::draw_dynamic_impl(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data)
{
    log.warning("Not yet implemented: chart_widget::draw_dynamic_impl");
}

} // namespace overlay
} // namespace video
} // namespace vgraph
