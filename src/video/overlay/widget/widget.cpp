#include "widget.h"

namespace vgraph {
namespace video {
namespace overlay {

widget::widget(unsigned int type):
    static_widget_(type & EType_Static),
    dynamic_widget_(type & EType_Dynamic)
{}

void widget::prepare(const std::vector<std::shared_ptr<telemetry::datapoint>>&)
{/*noop*/}

void widget::draw_static(cairo_t* cr)
{
    if (!static_widget_) {
        log.warning("Called draw_static on widget without static element");
        return;
    }

    cairo_save(cr);
    draw_static_impl(cr);
    cairo_restore(cr);
}

void widget::draw_dynamic(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data)
{
    if (!dynamic_widget_) {
        log.warning("Called draw_dynamic on widget without dynamic element");
        return;
    }

    cairo_save(cr);
    draw_dynamic_impl(cr, data);
    cairo_restore(cr);
}

bool widget::is_static() const
{
    return static_widget_;
}

bool widget::is_dynamic() const
{
    return dynamic_widget_;
}

void widget::draw_static_impl(cairo_t*)
{/*noop*/}

void widget::draw_dynamic_impl(cairo_t*, std::shared_ptr<telemetry::datapoint>)
{/*noop*/}

} // namespace overlay
} // namespace video
} // namespace vgraph
