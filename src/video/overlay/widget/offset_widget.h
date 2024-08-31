#ifndef OFFSET_WIDGET_H
#define OFFSET_WIDGET_H

#include "text_widget.h"

#include "telemetry/field.h"

namespace vgraph {
namespace video {
namespace overlay {

class offset_widget: public text_widget {
public:
    offset_widget(int x, int y, ETextAlign align, const std::string& font, const rgba& color, const rgba& border_color, int border_width, double offset);
    ~offset_widget();

private:
    utils::logging::logger log{"offset_widget"};

    void draw_volatile_impl(cairo_t* cr, double timestamp, double value) override;
    double get_volatile_value(double timestamp, const telemetry::timedatapoint&, const telemetry::timedatapoint&) override;

    double offset_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // OFFSET_WIDGET_H
