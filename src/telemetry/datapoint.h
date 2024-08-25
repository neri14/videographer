#ifndef DATAPOINT_H
#define DATAPOINT_H

#include "enum_field.h"
#include "value.h"

#include <map>
#include <vector>
#include <memory>

namespace vgraph {
namespace telemetry {

using datapoint = std::map<EField, value>;
using datapoint_sequence = std::vector<std::shared_ptr<datapoint>>;

} // namespace telemetry
} // namespace vgraph

#endif // DATAPOINT_H