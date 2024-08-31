#include "moving_chart_widget.h"

#include <cmath>

namespace vgraph {
namespace video {
namespace overlay {

moving_chart_widget::moving_chart_widget(int x, int y, int width, int height,
                                         const rgba& line_color, int line_width,
                                         const rgba& point_color, int point_size,
                                         const std::string& x_key, const std::string& y_key) :
    widget(EType_Static|EType_Dynamic),
    x_(x),
    y_(y),
    width_(width),
    height_(height),
    line_color_(line_color),
    line_width_(line_width),
    point_color_(point_color),
    point_radius_(point_size/2.0),
    x_field_(telemetry::map_key_to_field(x_key)),
    y_field_(telemetry::map_key_to_field(y_key))
{
    log.debug("Created chart widget");
}

moving_chart_widget::~moving_chart_widget()
{}

void moving_chart_widget::prepare(const std::vector<std::shared_ptr<telemetry::datapoint>>& datapoints)
{
    log.debug("Preloading chart");

    for (auto data : datapoints) {
        if (data->fields.contains(x_field_) && data->fields.contains(y_field_)) {
            double x = data->fields.at(x_field_);
            double y = data->fields.at(y_field_);

            min_x = std::min(min_x, x);
            max_x = std::max(max_x, x);

            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);

            points.push_back(std::make_pair(x, y));
        }
    }

    if (min_x < max_x && min_y < max_y) {
        scale_x = width_/(max_x-min_x);
        scale_y = -1 * height_/(max_y-min_y);

        valid = true;
        log.debug("Prepared data for generating widget");
    } else {
        log.warning("Not enough data points with data for charting");
    }
}

void moving_chart_widget::draw_static_impl(cairo_t* cr)
{
    if (!valid) {
        log.warning("Invalid widget state - ignoring");
        return;
    }

    cairo_set_source_rgba(cr, line_color_.r, line_color_.g, line_color_.b, line_color_.a);
    cairo_set_line_width(cr, line_width_);

    cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
    cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);

    int last_x = INT_MAX;
    int last_y = INT_MAX;
    for (auto [fx, fy] : points) {
        auto [x,y] = translate_xy(fx,fy);

        if (INT_MAX == last_x || INT_MAX == last_y) {
            cairo_move_to(cr, x, y);
        } else if (last_x != x || last_y != y) {
            cairo_line_to(cr, x, y);
        }

        last_x = x;
        last_y = y;
    }

    cairo_stroke(cr);
}

void moving_chart_widget::draw_dynamic_impl(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data)
{
    static bool valid = false;
    static double fx;
    static double fy;

    if (data->fields.contains(x_field_) && data->fields.contains(y_field_)) {
        fx = data->fields.at(x_field_);
        fy = data->fields.at(y_field_);
        valid = true;
    }

    if (valid) {
        auto [x,y] = translate_xy(fx, fy);

        cairo_move_to(cr, x, y + point_radius_);
        cairo_arc(cr, x, y, point_radius_, 0.0, 2*M_PI);

        cairo_set_source_rgba(cr, point_color_.r, point_color_.g, point_color_.b, point_color_.a);
        cairo_fill(cr);
    }
}

std::pair<int, int> moving_chart_widget::translate_xy(double x, double y)
{
    return std::make_pair(static_cast<int>(std::round(((x-min_x)*scale_x) + x_)),
                          static_cast<int>(std::round(((y-min_y)*scale_y) + y_ + height_)));

}

} // namespace overlay
} // namespace video
} // namespace vgraph
