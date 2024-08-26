#include "timecode_widget.h"

#include "color.h"
#include <iostream>

extern "C" {
    #include <pango/pangocairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

timecode_widget::timecode_widget(int center) :
    x_(center)
{}

timecode_widget::~timecode_widget()
{}

void timecode_widget::draw(cairo_t* cr, double timestamp)
{
    //save cairo state
    cairo_save(cr);

    //setup
    PangoLayout *layout = pango_cairo_create_layout(cr);

    //  set font
    PangoFontDescription *pfont = pango_font_description_from_string("DejaVu Sans 64");
    pango_layout_set_font_description(layout, pfont);
    pango_font_description_free(pfont);
    pfont = nullptr;

    //  set text and alignment
    pango_layout_set_text(layout, timestamp_str(timestamp).c_str(), -1);
    pango_layout_set_alignment(layout, PANGO_ALIGN_CENTER);

    //  setup offset depending on alignment
    int w,h;
    pango_layout_get_pixel_size(layout, &w, &h);

    int offset = w/2;

    // draw border
    cairo_move_to(cr, x_ - offset, y_);

    cairo_set_source_rgba(cr, color::black.r, color::black.g, color::black.b, color::black.a);
    cairo_set_line_width(cr, 4);

    pango_cairo_layout_path(cr, layout);
    cairo_stroke(cr);

    // draw background
    cairo_move_to(cr, x_ - offset, y_);

    cairo_set_source_rgba(cr, color::white.r, color::white.g, color::white.b, color::white.a);
    pango_cairo_show_layout (cr, layout);

    // cleanup
    g_object_unref(layout);

    //restore cairo state
    cairo_restore(cr);
}

std::string timecode_widget::timestamp_str(double timestamp) const
{
    double s = timestamp;
    int m = static_cast<int>(s)/60;
    int h = m/60;

    m = m-h*60;
    s = s-m*60;

    return std::move(std::format("{:02d}:{:02d}:{:09.06f}",h,m,s));
}

} // namespace overlay
} // namespace video
} // namespace vgraph
