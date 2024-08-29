#ifndef DATAPOINT_H
#define DATAPOINT_H

#include "field.h"

#include <map>
#include <vector>
#include <memory>
#include <chrono>

namespace vgraph {
namespace telemetry {

struct datapoint {
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    std::map<EField, double> fields;
};

using datapoint_ptr = std::shared_ptr<datapoint>;
using datapoint_seq = std::vector<datapoint_ptr>;

} // namespace telemetry
} // namespace vgraph

#endif // DATAPOINT_H