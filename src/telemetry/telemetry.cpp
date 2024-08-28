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

telemetry::telemetry(std::shared_ptr<datapoint_sequence> seq, long offset)
{
    if (!seq) {
        log.warning("No telemetry loaded");
        return;
    }

    auto first_timestamp = seq->front()->timestamp;
    auto prev_timestamp = seq->front()->timestamp;

    long sum = 0;
    
    bool first = true;
    for (auto& data : *seq) {
        long us = std::chrono::duration_cast<std::chrono::microseconds>(data->timestamp - first_timestamp).count();
        points[us+offset] = data;

        if (!first) {
            sum += std::chrono::duration_cast<std::chrono::microseconds>(data->timestamp - prev_timestamp).count();
        }

        prev_timestamp = data->timestamp;
        first = false;
    }

    avg_interval = first / seq->size();
}

std::shared_ptr<datapoint> telemetry::get(double timestamp) const
{
    long us = helper::in_microseconds(timestamp);

    if (points.begin()->first > us) {
        return nullptr;
    }
    if (points.rbegin()->first + avg_interval < us) {
        return nullptr;
    }

    return (--points.lower_bound(us))->second;
}

std::shared_ptr<telemetry> telemetry::load(const std::string& path, std::optional<double> offset)
{
    utils::logging::logger log{"telemetry::load"};

    std::shared_ptr<datapoint_sequence> seq;

    if (path.ends_with(".fit")) {
        seq = fit_parser().parse(path);
        if (!seq) {
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
