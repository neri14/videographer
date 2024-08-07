#ifndef FIT_PARSER_H
#define FIT_PARSER_H

#include "parser.h"

namespace vgraph {
namespace telemetry {

class fit_parser : public parser {
public:
    fit_parser() = default;
    ~fit_parser() = default;

    std::shared_ptr<datapoint_sequence> parse(const std::string& path) override;
};

} // namespace telemetry
} // namespace vgraph

#endif // FIT_PARSER_H
