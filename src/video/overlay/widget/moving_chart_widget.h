#ifndef MOVING_CHART_WIDGET_H
#define MOVING_CHART_WIDGET_H

#include "widget.h"
#include "color.h"
#include "text_align.h"

#include <string>
#include <cfloat>
#include <deque>

namespace vgraph {
namespace video {
namespace overlay {

class moving_chart_widget: public widget {
public:
    moving_chart_widget(int x, int y, int width, int height,
                        const rgba& line_color, int line_width,
                        const std::string& key, double window,
                        const std::string& font, const rgba& text_color, const rgba& text_border_color,
                        int text_border_width, const std::string& format, bool symmetric);
    ~moving_chart_widget();

    void prepare(const std::vector<std::shared_ptr<telemetry::datapoint>>& datapoints) override;

private:
    utils::logging::logger log{"moving_chart_widget"};

    void draw_volatile_impl(cairo_t* cr, double timestamp, double value) override;

    void draw_text(cairo_t* cr, int x, int y, double value);
    double get_volatile_value(double ts, const telemetry::timedatapoint& td_prev, const telemetry::timedatapoint& td_next) override;
    double translate(double value);

    int x_;
    int y_;
    int width_;
    int height_;
    rgba line_color_;
    int line_width_;

    telemetry::EField field_;
    double window_;

    std::string font_;
    rgba text_color_;
    rgba text_border_color_;
    int text_border_width_;
    std::string format_;

    bool symmetric_;

    std::vector<std::pair<double, double>> points;

    double min = DBL_MAX;
    double max = DBL_MIN;

    double scale = 0;
    double pix_per_s = 0;

    bool valid = false;
    std::deque<std::pair<double, double>> time_values;
};

} // namespace overlay
} // namespace video
} // namespace vgraph

#endif // MOVING_CHART_WIDGET_H
