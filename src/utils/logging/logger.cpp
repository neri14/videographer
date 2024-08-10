#include "logger.h"

namespace vgraph {
namespace utils {
namespace logging {

logger::logger(const std::string& name) :
    name_(name),
    backend_(backend::get_instance())
{}

} // namespace logging
} // namespace utils
} // namespace vgraph
