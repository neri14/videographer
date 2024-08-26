#ifndef WIDGET_H
#define WIDGET_H

#include "utils/logging/logger.h"

extern "C" {
    #include <cairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

class widget {
public:
    virtual ~widget() = default;

    void draw_static(cairo_t* cr);
    void draw_dynamic(cairo_t* cr, void* data); //FIXME void* data for future telemetry datapoint

    bool is_static() const;
    bool is_dynamic() const;

protected:
    widget(unsigned int type); // defined by bitwise or of EType values

    virtual void draw_static_impl(cairo_t* cr);
    virtual void draw_dynamic_impl(cairo_t* cr, void* data);

    utils::logging::logger log{"overlay"};

    enum EType {
        EType_Static = 0x1,
        EType_Dynamic = 0x2
    };

private:
    bool static_widget_;
    bool dynamic_widget_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif //WIDGET_H
