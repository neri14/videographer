#include "stream_sink.h"

#include <format>

namespace vgraph {
namespace utils {
namespace logging {

stream_sink::stream_sink(std::ostream& stream) :
    stream_(stream)
{}

void stream_sink::write(const std::string& msg)
{
    stream_ << std::format("{}\n", msg);
}

} // namespace logging
} // namespace utils
} // namespace vgraph
