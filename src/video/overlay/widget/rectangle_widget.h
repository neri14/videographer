#ifndef RECTANGLE_WIDGET_H
#define RECTANGLE_WIDGET_H

#include "widget.h"
#include "color.h"

namespace vgraph {
namespace video {
namespace overlay {

class rectangle_widget: public widget {
public:
    rectangle_widget(int x, int y, int width, int height, const rgba& bg_color, const rgba& border_color, int border_width);
    ~rectangle_widget();

private:
    utils::logging::logger log{"rectangle_widget"};

    void draw_static_impl(cairo_t* cr) override;

    int x_;
    int y_;
    int width_;
    int height_;
    rgba bg_color_;
    rgba border_color_;
    int border_width_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // RECTANGLE_WIDGET_H
