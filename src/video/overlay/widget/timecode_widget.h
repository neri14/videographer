#ifndef TIMECODE_WIDGET_H
#define TIMECODE_WIDGET_H

#include "text_widget.h"

extern "C" {
    #include <cairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

class timecode_widget : public text_widget {
public:
    timecode_widget(int center);
    ~timecode_widget();

    void draw_volatile_impl(cairo_t* cr, double timestamp) override;

private:
    std::string timestamp_str(double timestamp) const;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // TIMECODE_WIDGET_H
