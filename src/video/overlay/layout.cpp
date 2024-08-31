#include "layout.h"

#include "telemetry/telemetry.h"
#include "telemetry/field.h"
#include "widget/string_widget.h"
#include "widget/offset_widget.h"

#include <format>
#include <iostream>

namespace vgraph {
namespace video {
namespace overlay {
namespace consts {
    const std::pair<double, double> base_resolution(3840, 2160);

    const std::string font("DejaVu Sans {}");
    const int font_size(32);

    const int rows(30);
    const int header_row_y(150);
    const int first_row_y(250);
    const int row_height(50);

    const int timestamp_col_x(500);
    const int speed_col_x(1400);
    const int power_col_x(1650);
    const int offset_col_x(2000);
}

std::shared_ptr<layout> generate_alignment_layout(std::shared_ptr<telemetry::telemetry> tele, std::pair<int, int> resolution)
{
    double scale = std::min(resolution.first / consts::base_resolution.first, resolution.second / consts::base_resolution.second);

    int font_size = static_cast<int>(consts::font_size * scale);
    std::string font = std::vformat(consts::font, std::make_format_args(font_size));

    auto lay = std::make_shared<layout>();
    auto points = tele->get_all();
    auto first = points.front();

    lay->push_back(std::make_shared<string_widget>(
        consts::timestamp_col_x * scale, consts::header_row_y * scale, ETextAlign::Left, font, color::white, color::black, 4*scale, "timestamp"));

    lay->push_back(std::make_shared<string_widget>(
        consts::speed_col_x * scale, consts::header_row_y * scale, ETextAlign::Right, font, color::white, color::black, 4*scale, "speed"));

    lay->push_back(std::make_shared<string_widget>(
        consts::power_col_x * scale, consts::header_row_y * scale, ETextAlign::Right, font, color::white, color::black, 4*scale, "3s power"));

    lay->push_back(std::make_shared<string_widget>(
        consts::offset_col_x * scale, consts::header_row_y * scale, ETextAlign::Right, font, color::white, color::black, 4*scale, "offset"));

    int row = 0;
    for (const auto& point : points) {
        double row_y = consts::first_row_y * scale + consts::row_height * row * scale;

        lay->push_back(std::make_shared<string_widget>(
            consts::timestamp_col_x * scale, row_y, ETextAlign::Left, font, color::white, color::black, 4*scale,
            std::format("{:%Y-%m-%d %H:%M:%S} UTC", std::chrono::time_point_cast<std::chrono::seconds>(point->timestamp))));

        lay->push_back(std::make_shared<string_widget>(
            consts::speed_col_x * scale, row_y, ETextAlign::Right, font, color::white, color::black, 4*scale,
            std::format("{:.1f} km/h", point->fields.contains(telemetry::EField::Speed) ? point->fields.at(telemetry::EField::Speed) : 0.0)));

        lay->push_back(std::make_shared<string_widget>(
            consts::power_col_x * scale, row_y, ETextAlign::Right, font, color::white, color::black, 4*scale,
            std::format("{:.0f} W", point->fields.contains(telemetry::EField::Power3s) ? point->fields.at(telemetry::EField::Power3s) : 0.0)));

        lay->push_back(std::make_shared<offset_widget>(
            consts::offset_col_x * scale, row_y, ETextAlign::Right, font, color::white, color::black, 4*scale,
            std::chrono::duration_cast<std::chrono::microseconds>(point->timestamp - first->timestamp).count() / 1000000.0));
        
        if (++row > consts::rows) {
            break;
        }
    }

    return lay;
}

} // namespace overlay
} // namespace video
} // namespace vgraph
