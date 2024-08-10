#include "logger.h"

#include <format>

namespace vgraph {
namespace utils {
namespace logging {

logger::logger(const std::string& name) :
    name_(name),
    backend_(backend::get_instance())
{}

const void logger::debug(const std::string& msg)
{
    log(ELogLevel::Debug, msg);
}

const void logger::info(const std::string& msg)
{
    log(ELogLevel::Info, msg);
}

const void logger::warning(const std::string& msg)
{
    log(ELogLevel::Warning, msg);
}

const void logger::error(const std::string& msg)
{
    log(ELogLevel::Error, msg);
}

const void logger::log(ELogLevel level, const std::string& msg)
{
    backend_.log(name_, level, msg);
}

} // namespace logging
} // namespace utils
} // namespace vgraph
