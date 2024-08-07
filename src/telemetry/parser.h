#ifndef FILE_H
#define FILE_H

#include "datapoint.h"

#include <string>
#include <memory>

namespace vgraph {
namespace telemetry {

class parser {
public:
    parser() = default;
    ~parser() = default;

    virtual std::shared_ptr<datapoint_sequence> parse(const std::string& path) = 0;
};

} // namespace telemetry
} // namespace vgraph

#endif // FILE_H
