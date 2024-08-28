#ifndef VALUE_WIDGET_H
#define VALUE_WIDGET_H

#include "text_widget.h"

#include "telemetry/field.h"

namespace vgraph {
namespace video {
namespace overlay {

class value_widget: public text_widget {
public:
    value_widget(int x, int y, ETextAlign align, const std::string& font, const rgba& color, const rgba& border_color, int border_width, const std::string& key, int precision);
    ~value_widget();

private:
    utils::logging::logger log{"value_widget"};

    void draw_dynamic_impl(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data) override;

    telemetry::EField field_;
    int precision_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // VALUE_WIDGET_H
