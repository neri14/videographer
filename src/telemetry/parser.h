#ifndef FILE_H
#define FILE_H

#include "datapoint.h"

#include "utils/logging/logger.h"

#include <filesystem>
#include <map>

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

    double gradient_between(const std::multimap<int, datapoint_ptr>& dist_points, int dist_a, int dist_b);

    //last - if there are multiple values for key, return last, otherwise return first
    double get_by_distance(const std::multimap<int, datapoint_ptr>& dist_points, int dist, EField field, bool last=true);

    void set_if_ok(datapoint_ptr& data, EField field, std::optional<double> value);
    std::optional<double> field_avg(datapoint_seq::const_reverse_iterator it,
                                    datapoint_seq::const_reverse_iterator rend,
                                    EField field, int count);
};

} // namespace telemetry
} // namespace vgraph

#endif // FILE_H
