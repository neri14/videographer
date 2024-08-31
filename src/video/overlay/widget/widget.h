#ifndef WIDGET_H
#define WIDGET_H

#include "utils/logging/logger.h"
#include "telemetry/datapoint.h"
#include "telemetry/telemetry.h"

#include <functional>

extern "C" {
    #include <cairo.h>
}

namespace vgraph {
namespace video {
namespace overlay {

class widget {
public:
    virtual ~widget() = default;

    virtual void prepare(const std::vector<std::shared_ptr<telemetry::datapoint>>& points);

    void draw_static(cairo_t* cr);
    void draw_dynamic(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data);
    void draw_volatile(cairo_t* cr, double timestamp, const telemetry::timedatapoint& timedata_prev, const telemetry::timedatapoint& timedata_next);

    bool is_static() const;
    bool is_dynamic() const;
    bool is_volatile() const;

protected:
    widget(unsigned int type); // type defined by bitwise or of EType values

    virtual void draw_static_impl(cairo_t* cr);
    virtual void draw_dynamic_impl(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data);
    virtual void draw_volatile_impl(cairo_t* cr, double timestamp, double value);

    virtual double get_volatile_value(double timestamp, const telemetry::timedatapoint& timedata_prev, const telemetry::timedatapoint& timedata_next);

    enum EType {
        EType_Static = 0x1,
        EType_Dynamic = 0x2,
        EType_Volatile = 0x4
    };

private:
    utils::logging::logger log{"widget"};
    bool static_widget_;
    bool dynamic_widget_;
    bool volatile_widget_;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif //WIDGET_H
