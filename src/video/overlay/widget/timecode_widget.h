#ifndef TIMECODE_WIDGET_H
#define TIMECODE_WIDGET_H

#include <string>

extern "C" {
    #include <cairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

class timecode_widget {
public:
    timecode_widget(int center);
    ~timecode_widget();

    void draw(cairo_t* cr, double timestamp);

private:
    std::string timestamp_str(double timestamp) const;

    int x_;
    int y_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // TIMECODE_WIDGET_H
