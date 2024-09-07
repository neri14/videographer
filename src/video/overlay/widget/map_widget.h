#ifndef MAP_WIDGET_H
#define MAP_WIDGET_H

#include "widget.h"
#include "color.h"

#include <string>
#include <cfloat>

namespace vgraph {
namespace video {
namespace overlay {

class map_widget: public widget {
public:
    map_widget(int x, int y, int width, int height,
                 const rgba& line_color, int line_width,
                 const rgba& point_color, int point_size);
    ~map_widget();

    void prepare(const std::vector<std::shared_ptr<telemetry::datapoint>>& datapoints) override;

private:
    utils::logging::logger log{"map_widget"};

    void draw_static_impl(cairo_t* cr) override;
    void draw_dynamic_impl(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data) override;

    std::pair<double, double> project(double x, double y);
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

    std::vector<std::pair<double, double>> points;

    double min_x = DBL_MAX;
    double max_x = DBL_MIN;
    double min_y = DBL_MAX;
    double max_y = DBL_MIN;

    double scale = 0;
    double offset_x = 0;
    double offset_y = 0;
    bool valid = false;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // MAP_WIDGET_H
