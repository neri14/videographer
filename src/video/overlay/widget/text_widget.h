#ifndef TEXT_WIDGET_H
#define TEXT_WIDGET_H

#include "widget.h"
#include "color.h"
#include "text_align.h"

#include <string>

namespace vgraph {
namespace video {
namespace overlay {

class text_widget: public widget {
public:
    text_widget(unsigned int type, int x, int y, ETextAlign align, const std::string& font,
                const rgba& color, const rgba& border_color, int border_width,
                volatile_value_fun_t vfun=volatile_value_fun_t());
    ~text_widget();

protected:
    virtual void draw_text(cairo_t* cr, const std::string& str);

private:
    utils::logging::logger log{"text_widget"};

    int x_;
    int y_;
    std::string font_;
    rgba color_;
    rgba border_color_;
    int border_width_;
    ETextAlign align_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // TEXT_WIDGET_H
