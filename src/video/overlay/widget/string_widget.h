#ifndef STRING_WIDGET_H
#define STRING_WIDGET_H

#include "text_widget.h"

namespace vgraph {
namespace video {
namespace overlay {

class string_widget: public text_widget {
public:
    string_widget(int x, int y, ETextAlign align, const std::string& font, const rgba& color, const rgba& border_color, int border_width, const std::string& str);
    ~string_widget();

private:
    utils::logging::logger log{"string_widget"};

    void draw_static_impl(cairo_t* cr) override;

    std::string str_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // STRING_WIDGET_H
