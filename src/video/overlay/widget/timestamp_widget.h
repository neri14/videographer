#ifndef TIMESTAMP_WIDGET_H
#define TIMESTAMP_WIDGET_H

#include "text_widget.h"
#include "color.h"
#include "text_align.h"

#include <string>

namespace vgraph {
namespace video {
namespace overlay {

class timestamp_widget: public text_widget {
public:
    timestamp_widget(int x, int y, ETextAlign align, const std::string& font, const rgba& color, const rgba& border_color, int border_width, const std::string& format, int utcoffset);
    ~timestamp_widget();

private:
    utils::logging::logger log{"timestamp_widget"};

    void draw_dynamic_impl(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data) override;

    std::string format_;
    int utcoffset_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // TIMESTAMP_WIDGET_H
