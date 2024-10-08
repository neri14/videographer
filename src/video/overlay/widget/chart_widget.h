#ifndef CHART_WIDGET_H
#define CHART_WIDGET_H

#include "widget.h"
#include "color.h"

#include <string>
#include <cfloat>

namespace vgraph {
namespace video {
namespace overlay {

class chart_widget: public widget {
public:
    chart_widget(int x, int y, int width, int height,
                 const rgba& line_color, int line_width,
                 const rgba& point_color, int point_size,
                 const std::string& x_key, const std::string& y_key);
    ~chart_widget();

    void prepare(const std::vector<std::shared_ptr<telemetry::datapoint>>& datapoints) override;

private:
    utils::logging::logger log{"chart_widget"};

    void draw_static_impl(cairo_t* cr) override;
    void draw_dynamic_impl(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data) override;

    std::pair<int, int> translate_xy(double x, double y);

    int x_;
    int y_;
    int width_;
    int height_;
    rgba line_color_;
    int line_width_;
    rgba point_color_;
    double point_radius_;
    std::string x_key_;
    std::string y_key_;
    telemetry::EField x_field_;
    telemetry::EField y_field_;

    std::vector<std::pair<double, double>> points;

    double min_x = DBL_MAX;
    double max_x = DBL_MIN;
    double min_y = DBL_MAX;
    double max_y = DBL_MIN;

    double scale_x = 0;
    double scale_y = 0;
    bool valid = false;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // CHART_WIDGET_H
