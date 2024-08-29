#include "telemetry.h"
#include "fit_parser.h"
#include <cmath>

namespace vgraph {
namespace telemetry {
namespace helper {
    long in_microseconds(double t)
    {
        return std::lround(t * 1000000);
    }
}

telemetry::telemetry(datapoint_seq seq, long offset) :
    points(seq)
{
    if (points.empty()) {
        log.warning("No telemetry loaded");
        return;
    }

    auto first_timestamp = points.front()->timestamp;
    auto prev_timestamp = points.front()->timestamp;

    long sum = 0;
    
    bool first = true;
    for (auto& data : points) {
        long us = std::chrono::duration_cast<std::chrono::microseconds>(data->timestamp - first_timestamp).count();
        time_points[us+offset] = data;

        if (!first) {
            sum += std::chrono::duration_cast<std::chrono::microseconds>(data->timestamp - prev_timestamp).count();
        }

        prev_timestamp = data->timestamp;
        first = false;
    }

    avg_interval = first / points.size();
}

datapoint_ptr telemetry::get(double timestamp) const
{
    long us = helper::in_microseconds(timestamp);

    if (time_points.begin()->first > us) {
        return nullptr;
    }
    if (time_points.rbegin()->first + avg_interval < us) {
        return nullptr;
    }

    return (--time_points.lower_bound(us))->second;
}

const std::vector<datapoint_ptr>& telemetry::get_all() const
{
    return points;
}

std::shared_ptr<telemetry> telemetry::load(const std::string& path, std::optional<double> offset)
{
    utils::logging::logger log{"telemetry::load"};

    datapoint_seq seq;

    if (path.ends_with(".fit")) {
        seq = fit_parser().parse(path);
        if (seq.empty()) {
            log.error("Failed to parse FIT file: {}", path);
            return nullptr;
        }
    } else {
        log.error("Unsupported telemetry file format: {}", path);
        return nullptr;
    }

    return std::make_shared<telemetry>(seq, offset ? helper::in_microseconds(*offset) : 0);
}

} // namespace telemetry
} // namespace vgraph
