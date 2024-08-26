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

telemetry::telemetry(std::shared_ptr<datapoint_sequence> seq, long offset):
    offset_(offset)
{
    if (!seq) {
        log.warning("No telemetry loaded");
        return;
    }

    for (auto& dp : *seq) {
        //TODO parse into internal representation keyed on usecond timestamps
        //TODO store averge time between points (for last point retreiveal logic)
    }
}

std::shared_ptr<datapoint> telemetry::get(double timestamp) const
{
    //TODO retrieval of datapoint from internal representation to be implemented
    // if before first_point:
    //     return nullptr;
    // if last_point and (argument::timestamp - last_point.timestamp) > average_time_between_points:
    //     return nullptr;
    // else:
    //     return point with greatest point.timestamp where: point.timestamp <= argument::timestamp
    return nullptr;
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
