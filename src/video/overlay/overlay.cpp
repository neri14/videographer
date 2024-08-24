#include "overlay.h"

#include <format>
#include <chrono>

extern "C" {
    #include <pango/pangocairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

overlay::~overlay()
{
    double total_s = total_drawing_time/1000000.0;

    log.info("Total drawing time: {:.6f} s", total_s);
    log.info("Average drawing time: {:.6f} s/frame", total_s/total_drawn_frames);
}

std::string timestamp_str(double timestamp)
{
    double s = timestamp;
    int m = static_cast<int>(s)/60;
    int h = m/60;

    m = m-h*60;
    s = s-m*60;

    return std::format("{:02d}:{:02d}:{:09.06f}",h,m,s);
}

void overlay::draw(cairo_t* cr, double timestamp)
{
    auto t1 = std::chrono::high_resolution_clock::now();

    PangoLayout *layout = pango_cairo_create_layout(cr);

    PangoFontDescription *font = pango_font_description_from_string("Sans Bold 27");
    pango_layout_set_font_description (layout, font);
    pango_font_description_free (font);
    font = nullptr;

    pango_layout_set_text (layout, timestamp_str(timestamp).c_str(), -1);

    cairo_save(cr);
    cairo_move_to(cr, 100, 100);
    cairo_set_source_rgb(cr, 0, 0, 0);
    pango_cairo_show_layout (cr, layout);
    cairo_restore(cr);

    g_object_unref(layout);

    total_drawing_time += std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - t1).count();
    total_drawn_frames++;
}

} // namespace overlay
} // namespace video
} // namespace vgraph
