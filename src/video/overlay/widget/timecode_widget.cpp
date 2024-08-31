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
    text_widget(EType_Volatile,
                center, 10,
                ETextAlign::Center,
                "DejaVu Sans 64",
                color::white, color::black, 4,
                [](double timestamp, telemetry::timedatapoint, telemetry::timedatapoint)->double { return timestamp; })
{}

timecode_widget::~timecode_widget()
{}

void timecode_widget::draw_volatile_impl(cairo_t* cr, double timestamp)
{
    draw_text(cr, timestamp_str(timestamp).c_str());
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
