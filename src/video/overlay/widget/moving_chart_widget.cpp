#include "moving_chart_widget.h"

#include <cmath>

#include <iostream>

namespace vgraph {
namespace video {
namespace overlay {

moving_chart_widget::moving_chart_widget(int x, int y, int width, int height,
                                         const rgba& line_color, int line_width,
                                         const rgba& point_color, int point_size,
                                         const std::string& key, double window) :
    widget(EType_Dynamic),//FIXME replace with Volatile
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
    pix_per_s(width/window),
    val_y(height)
{
    log.debug("Created chart widget");
}

moving_chart_widget::~moving_chart_widget()
{
    if (cache) {
        cairo_surface_destroy(cache);
        cache = nullptr;
    }
}

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

void moving_chart_widget::draw_dynamic_impl(cairo_t* cr, std::shared_ptr<telemetry::datapoint> data)
{
    if (!valid) {
        return;
    }

    double val = data->fields.contains(field_) ? data->fields.at(field_) : 0.0;

    cairo_surface_t* next = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_, height_);
    cairo_t* next_cr = cairo_create(next);

    //draw 'old cache' onto 'new cache'
    if (cache) {
        cairo_set_source_surface(next_cr, cache, -pix_per_s, 0);
        cairo_paint(next_cr);
        cairo_surface_destroy(cache);
        cache = nullptr;
    }

    //draw new line seg ment onto 'next cache'
    cairo_set_source_rgba(next_cr, line_color_.r, line_color_.g, line_color_.b, line_color_.a);
    cairo_set_line_width(next_cr, line_width_);

    cairo_set_line_cap(next_cr, CAIRO_LINE_CAP_SQUARE);
    cairo_set_line_join(next_cr, CAIRO_LINE_JOIN_BEVEL);

    cairo_move_to(next_cr, width_-pix_per_s, val_y);
    val_y = translate(val);
    cairo_line_to(next_cr, width_, val_y);

    cairo_stroke(next_cr);

    //draw 'next cache' onto screen
    cairo_set_source_surface(cr, next, x_, y_);
    cairo_paint(cr);

    //store data for next iteration
    cache = next;
}

double moving_chart_widget::translate(double value)
{
    return ((value - min) * scale) + height_;
}

} // namespace overlay
} // namespace video
} // namespace vgraph
