#include "backend.h"

#include <format>
#include <map>

namespace vgraph {
namespace utils {
namespace logging {

namespace consts {
const std::map<ELogLevel, std::string> level_str = {
    {ELogLevel::Debug,   "DBG"},
    {ELogLevel::Info,    "INF"},
    {ELogLevel::Warning, "WRN"},
    {ELogLevel::Error,   "ERR"}
};
} // namespace consts


backend& backend::get_instance()
{
    static backend instance;
    return instance;
}

const void backend::log(const std::string& logger_name, ELogLevel level, const std::string& msg)
{
    std::string fmsg = std::format("[{}] {}: {}", consts::level_str.at(level), logger_name, msg);

    for(auto [s_level, s] : sinks_) {
        if (s_level <= level)
            s->write(fmsg);
    }
}

void backend::add_sink(std::shared_ptr<sink> sink, ELogLevel level)
{
    sinks_.push_back(std::make_pair(level, sink));
}

void backend::set_log_level(std::shared_ptr<sink> sink, ELogLevel level)
{
    std::for_each(sinks_.begin(), sinks_.end(), [sink, level](auto& s) {
        if (s.second == sink)
            s.first = level;
    });
}

void backend::remove_sink(std::shared_ptr<sink> sink)
{
    sinks_.erase(std::remove_if(sinks_.begin(), sinks_.end(), [sink](auto& s) {
        return s.second == sink;
    }), sinks_.end());
}

} // namespace logging
} // namespace utils
} // namespace vgraph
