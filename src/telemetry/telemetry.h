#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <memory>

#include "utils/logging/logger.h"
#include "datapoint.h"

namespace vgraph {
namespace telemetry {

class telemetry {
public:
    telemetry(std::shared_ptr<datapoint_sequence> seq, long offset=0);
    ~telemetry() = default;

    std::shared_ptr<datapoint> get(double timestamp) const;
    const std::vector<std::shared_ptr<datapoint>>& get_all() const;

    static std::shared_ptr<telemetry> load(const std::string& path, std::optional<double> offset); // optional offset in seconds

private:
    utils::logging::logger log{"telemetry"};

    long avg_interval = 0;

    std::vector<std::shared_ptr<datapoint>> points;
    std::map<long, std::shared_ptr<datapoint>> time_points;
};

} // namespace telemetry
} // namespace vgraph

#endif // TELEMETRY_H