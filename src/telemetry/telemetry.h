#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <memory>

#include "utils/logging/logger.h"
#include "datapoint.h"

namespace vgraph {
namespace telemetry {

class telemetry {
public:
    telemetry(datapoint_seq seq, long offset=0);
    ~telemetry() = default;

    datapoint_ptr get(double timestamp) const;
    const datapoint_seq& get_all() const;

    static std::shared_ptr<telemetry> load(const std::string& path, std::optional<double> offset); // optional offset in seconds

private:
    utils::logging::logger log{"telemetry"};

    long avg_interval = 0;

    datapoint_seq points;
    std::map<long, datapoint_ptr> time_points;
};

} // namespace telemetry
} // namespace vgraph

#endif // TELEMETRY_H