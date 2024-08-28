#include "map_widget.h"

#include "telemetry/field.h"

#include <numbers>
#include <cmath>

namespace vgraph {
namespace video {
namespace overlay {
namespace consts {
    const double mercator_w(36000.0);
    const double mercator_h(18000.0);

    const telemetry::EField x_field(telemetry::EField::Longitude);
    const telemetry::EField y_field(telemetry::EField::Latitude);
}

map_widget::map_widget(int x, int y, int width, int height,
                       const rgba& line_color, int line_width,
                       const rgba& point_color, int point_size) :
    widget(EType_Static|EType_Dynamic),
    x_(x),
    y_(y),
    width_(width),
    height_(height),
    line_color_(line_color),
    line_width_(line_width),
    point_color_(point_color),
    point_radius_(point_size/2.0)
{
    log.debug("Created map widget");
}

map_widget::~map_widget()
{}

void map_widget::prepare(const std::vector<std::shared_ptr<telemetry::datapoint>>& datapoints)
{
    log.debug("Preloading map");

    for (auto data : datapoints) {
        if (data->fields.contains(consts::x_field) && data->fields.contains(consts::y_field)) {
            auto [x,y] = project(data->fields.at(consts::x_field), data->fields.at(consts::y_field));

            min_x = std::min(min_x, x);
            max_x = std::max(max_x, x);

            min_y = std::min(min_y, y);
            max_y = std::max(max_y, y);

            points.push_back(std::make_pair(x, y));
        }
    }

    if (min_x < max_x && min_y < max_y) {
        scale_x = width_/(max_x-min_x);
        scale_y = height_/(max_y-min_y);

        valid = true;
        log.debug("Prepared data for generating widget");
    } else {
        log.warning("Not enough data points with data for creating a map");
    }
}

void map_widget::draw_static_impl(cairo_t* cr)
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

void map_widget::draw_dynamic_impl(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data)
{
    if (data->fields.contains(consts::x_field) && data->fields.contains(consts::y_field)) {
        auto [fx,fy] = project(data->fields.at(consts::x_field), data->fields.at(consts::y_field));
        auto [x,y] = translate_xy(fx, fy);

        cairo_move_to(cr, x, y + point_radius_);
        cairo_arc(cr, x, y, point_radius_, 0.0, 2*M_PI);

        cairo_set_source_rgba(cr, point_color_.r, point_color_.g, point_color_.b, point_color_.a);
        cairo_fill(cr);
    }
}

std::pair<int, int> map_widget::translate_xy(double x, double y)
{
    return std::make_pair(static_cast<int>(std::round(((x-min_x)*scale_x) + x_)),
                          static_cast<int>(std::round(((y-min_y)*scale_y) + y_)));
}

std::pair<double, double> map_widget::project(double x, double y)
{
    x = (x + 180) * (consts::mercator_w / 360);
    
    double lat_rad = y * std::numbers::pi / 180;
    double merc_n = std::log(std::tan((std::numbers::pi/4)+(lat_rad/2)));
    y = consts::mercator_h / 2 - consts::mercator_w * merc_n / (2 * std::numbers::pi);

    return std::make_pair(x, y);
}

} // namespace overlay
} // namespace video
} // namespace vgraph
