#include "string_widget.h"

#include <iostream>

extern "C" {
    #include <pango/pangocairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

string_widget::string_widget(int x,
                             int y,
                             const std::string& text,
                             const std::string& font,
                             const rgba& color,
                             const rgba& border_color,
                             int border_width,
                             ETextAlign align) :
    widget::widget(EType_Static),
    x_(x),
    y_(y),
    text_(text),
    font_(font),
    color_(color),
    border_color_(border_color),
    border_width_(border_width),
    align_(align)
{}

string_widget::~string_widget()
{}

void string_widget::draw_static_impl(cairo_t* cr)
{
    //setup
    PangoLayout *layout = pango_cairo_create_layout(cr);

    //  set font
    PangoFontDescription *pfont = pango_font_description_from_string(font_.c_str());
    pango_layout_set_font_description(layout, pfont);
    pango_font_description_free(pfont);
    pfont = nullptr;

    //  set text and alignment
    pango_layout_set_text(layout, text_.c_str(), -1);
    pango_layout_set_alignment(layout, to_pango_align(align_));

    //  setup offset depending on alignment
    int w,h;
    pango_layout_get_pixel_size(layout, &w, &h);

    int offset = 0;
    if (align_ == ETextAlign::Right) {
        offset = w;
    } else if (align_ == ETextAlign::Center) {
        offset = w/2;
    }

    // draw border
    cairo_move_to(cr, x_ - offset, y_);

    cairo_set_source_rgba(cr, border_color_.r, border_color_.g, border_color_.b, border_color_.a);
    cairo_set_line_width(cr, border_width_*2);

    pango_cairo_layout_path(cr, layout);
    cairo_stroke(cr);

    // draw background
    cairo_move_to(cr, x_ - offset, y_);

    cairo_set_source_rgba(cr, color_.r, color_.g, color_.b, color_.a);
    pango_cairo_show_layout (cr, layout);

    // cleanup
    g_object_unref(layout);
}

} // namespace overlay
} // namespace video
} // namespace vgraph
