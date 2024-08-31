#include "telemetry.h"
#include "fit_parser.h"
#include <cmath>

namespace vgraph {
namespace telemetry {
namespace consts {
    double us(1000000);
}
namespace helper {
    long s_to_us(double t)
    {
        return std::lround(t * consts::us);
    }
    double us_to_s(long t)
    {
        return t / consts::us;
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
    long us = helper::s_to_us(timestamp);

    if (time_points.begin()->first > us) {
        return nullptr;
    }
    if (time_points.rbegin()->first + avg_interval < us) {
        return nullptr;
    }

    auto it = time_points.lower_bound(us);
    if (it->first > us) {
        it--;
    }
    return it->second;
}

timedatapoint telemetry::get_td_prev(double timestamp) const
{
    long us = helper::s_to_us(timestamp);
    if (time_points.begin()->first > us) {
        return timedatapoint(0, nullptr);
    }
    if (time_points.rbegin()->first < us) {
        auto it = time_points.rbegin();
        return timedatapoint(helper::us_to_s(it->first), it->second);
    }

    auto it = time_points.lower_bound(us);
    if (it->first > us) {
        it--;
    }
    return timedatapoint(helper::us_to_s(it->first), it->second);
}

timedatapoint telemetry::get_td_next(double timestamp) const
{
    long us = helper::s_to_us(timestamp);
    if (time_points.rbegin()->first < us) {
        return timedatapoint(0, nullptr);
    }
    if (time_points.begin()->first > us) {
        auto it = time_points.begin();
        return timedatapoint(helper::us_to_s(it->first), it->second);
    }
    auto it = time_points.lower_bound(us);
    return timedatapoint(helper::us_to_s(it->first), it->second);
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

    return std::make_shared<telemetry>(seq, offset ? helper::s_to_us(*offset) : 0);
}

} // namespace telemetry
} // namespace vgraph
