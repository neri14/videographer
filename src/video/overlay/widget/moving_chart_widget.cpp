#include "moving_chart_widget.h"

#include <cmath>

#include <iostream>

namespace vgraph {
namespace video {
namespace overlay {
namespace consts {
    const double margin(5);
}

moving_chart_widget::moving_chart_widget(int x, int y, int width, int height,
                                         const rgba& line_color, int line_width,
                                         const rgba& point_color, int point_size,
                                         const std::string& key, double window) :
    widget(EType_Volatile),//FIXME replace with Volatile
    x_(x),
    y_(y),
    width_(width),
    height_(height),
    line_color_(line_color),
    line_width_(line_width),
    point_color_(point_color),
    point_radius_(point_size/2.0),
    field_(telemetry::map_key_to_field(key)),
    window_(window),
    pix_per_s(width/window)
{
    log.debug("Created chart widget");
}

moving_chart_widget::~moving_chart_widget()
{}

//FIXME precalculate smoothed out values with <time, value> pairs for each time pixel has to be redrawn?
void moving_chart_widget::prepare(const std::vector<std::shared_ptr<telemetry::datapoint>>& datapoints)
{
    log.debug("Preparing moving chart metadata");

    for (auto data : datapoints) {
        double val = 0;
        if (data->fields.contains(field_)) {
            val = data->fields.at(field_);
        }

        min = std::min(min, val);
        max = std::max(max, val);
    }

    if (min < max) {
        scale = -1 * height_/(max-min);
        valid = true;
        log.debug("Prepared data for generating widget");
    } else {
        log.warning("Not enough data points with data for charting");
    }
}

void moving_chart_widget::draw_volatile_impl(cairo_t* cr, double ts, double value)
{
    if (!valid) {
        return;
    }

    // cleanup values no longer needed
    while (!time_values.empty() && ts - time_values.front().first > window_) {
        time_values.pop_front();
    }
    time_values.push_back(std::make_pair(ts, translate(value)));

    cairo_surface_t* cache = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_, height_ + 2*consts::margin);
    cairo_t* cache_cr = cairo_create(cache);

    //setup line config
    cairo_set_source_rgba(cache_cr, line_color_.r, line_color_.g, line_color_.b, line_color_.a);
    cairo_set_line_width(cache_cr, line_width_);
    cairo_set_line_cap(cache_cr, CAIRO_LINE_CAP_SQUARE);
    cairo_set_line_join(cache_cr, CAIRO_LINE_JOIN_ROUND);

    //draw path
    bool first = true;
    for (auto [t, y] : time_values) {
        double x = width_ - width_ * (ts - t) / window_;
        y += consts::margin;
        if (first) {
            cairo_move_to(cache_cr, x, y);
            first = false;
        } else {
            cairo_line_to(cache_cr, x, y);
        }
    }
    cairo_stroke(cache_cr);

    //draw 'next cache' onto screen
    cairo_set_source_surface(cr, cache, x_, y_ - consts::margin);
    cairo_paint(cr);
}

double moving_chart_widget::get_volatile_value(double ts, const telemetry::timedatapoint& td_prev, const telemetry::timedatapoint& td_next)
{
    if (td_prev.second == td_next.second) {
        return td_prev.second->fields.contains(field_) ? 
               td_prev.second->fields.at(field_) : 0;
    }

    double t_a = td_prev.first;
    double t_b = td_next.first;

    double a = 0;
    double b = 0;
    if (td_prev.second->fields.contains(field_)) {
        a = td_prev.second->fields.at(field_);
    }
    if (td_next.second->fields.contains(field_)) {
        b = td_next.second->fields.at(field_);
    }

    double ratio_a = (t_b - ts) / (t_b - t_a);
    double ratio_b = 1 - ratio_a;

    return a * ratio_a + b * ratio_b;
}

double moving_chart_widget::translate(double value)
{
    return ((value - min) * scale) + height_;
}

} // namespace overlay
} // namespace video
} // namespace vgraph
