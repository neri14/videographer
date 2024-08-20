#ifndef backend_H
#define backend_H

#include "log_level.h"
#include "sink.h"

#include <string>
#include <memory>
#include <vector>
#include <utility>

namespace vgraph {
namespace utils {
namespace logging {

class backend {
public:
    static backend& get_instance();

    const void log(const std::string& logger_name, ELogLevel level, const std::string& msg);

    void add_sink(std::shared_ptr<sink> sink, ELogLevel level = ELogLevel::Debug);
    void set_log_level(std::shared_ptr<sink> sink, ELogLevel level);
    void remove_sink(std::shared_ptr<sink> sink);

private:
    backend() = default;
    ~backend() = default;
    backend(const backend&) = delete;

    std::vector<std::pair<ELogLevel, std::shared_ptr<sink>>> sinks_;
};

} // namespace logging
} // namespace utils
} // namespace vgraph

#endif // backend_H
