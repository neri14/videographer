#ifndef FILE_H
#define FILE_H

#include "datapoint.h"

#include "utils/logging/logger.h"

#include <filesystem>
#include <memory>
#include <deque>

namespace vgraph {
namespace telemetry {

class parser {
public:
    parser() = default;
    ~parser() = default;

    std::shared_ptr<datapoint_sequence> parse(const std::filesystem::path& path);

protected:
    virtual std::shared_ptr<datapoint_sequence> parse_impl(const std::filesystem::path& path) = 0;

private:
    utils::logging::logger log{"parser"};

    void update_calculated_fields(std::shared_ptr<datapoint_sequence>& seq);
    void print_stats(std::shared_ptr<datapoint_sequence>& seq);

    void set_if_ok(std::shared_ptr<datapoint>& data, EField field, std::optional<double> value);
    std::optional<double> field_avg(datapoint_sequence::reverse_iterator it, datapoint_sequence::reverse_iterator rend, EField field, int count);
};

} // namespace telemetry
} // namespace vgraph

#endif // FILE_H
