#ifndef STRING_WIDGET_H
#define STRING_WIDGET_H

#include "widget.h"
#include "color.h"
#include "text_align.h"

#include <string>

namespace vgraph {
namespace video {
namespace overlay {

class string_widget: public widget {
public:
    string_widget(int x, int y, const std::string& text, const std::string& font, const rgba& color, const rgba& border_color, int border_width, ETextAlign align);
    ~string_widget() = default;

private:
    void draw_impl(cairo_t* cr) override;

    int x_;
    int y_;
    std::string text_;
    std::string font_;
    rgba color_;
    rgba border_color_;
    int border_width_;
    ETextAlign align_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // STRING_WIDGET_H
