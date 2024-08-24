#ifndef WIDGET_H
#define WIDGET_H

extern "C" {
    #include <cairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

class widget {
public:
    widget();
    virtual ~widget() = default;

    void draw(cairo_t* cr);//TODO add second argument with telemetry datapoint

protected:
    virtual void draw_impl(cairo_t* cr) = 0;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif //WIDGET_H
