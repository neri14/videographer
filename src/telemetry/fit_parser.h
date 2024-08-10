#ifndef FIT_PARSER_H
#define FIT_PARSER_H

#include "parser.h"

#include "utils/logging/logger.h"

namespace vgraph {
namespace telemetry {

class fit_parser : public parser {
public:
    fit_parser() = default;
    ~fit_parser() = default;

    std::shared_ptr<datapoint_sequence> parse(const std::filesystem::path& path) override;

private:
    utils::logging::logger log{"fit_parser"};
};

} // namespace telemetry
} // namespace vgraph

#endif // FIT_PARSER_H
