#ifndef LINE_WIDGET_H
#define LINE_WIDGET_H

#include "widget.h"
#include "color.h"

namespace vgraph {
namespace video {
namespace overlay {

class line_widget: public widget {
public:
    line_widget(int x1, int y1, int x2, int y2, const rgba& color, int width);
    ~line_widget();

private:
    utils::logging::logger log{"line_widget"};

    void draw_static_impl(cairo_t* cr) override;

    int x1_;
    int y1_;
    int x2_;
    int y2_;
    rgba color_;
    int width_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // LINE_WIDGET_H
