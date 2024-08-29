#ifndef FILE_H
#define FILE_H

#include "datapoint.h"

#include "utils/logging/logger.h"

#include <filesystem>

namespace vgraph {
namespace telemetry {

class parser {
public:
    parser() = default;
    ~parser() = default;

    datapoint_seq parse(const std::filesystem::path& path);

protected:
    virtual datapoint_seq parse_impl(const std::filesystem::path& path) = 0;

private:
    utils::logging::logger log{"parser"};

    void update_calculated_fields(datapoint_seq& seq);
    void print_stats(datapoint_seq& seq);

    double gradient_between(const datapoint_seq& seq, double dist_a, double dist_b);
    std::vector<double> get_by_distance(const datapoint_seq& seq, double dist, EField field);

    void set_if_ok(datapoint_ptr& data, EField field, std::optional<double> value);
    std::optional<double> field_avg(datapoint_seq::const_reverse_iterator it,
                                    datapoint_seq::const_reverse_iterator rend,
                                    EField field, int count);
};

} // namespace telemetry
} // namespace vgraph

#endif // FILE_H
