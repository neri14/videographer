#ifndef LOGGER_H
#define LOGGER_H

#include <string>

#include "log_level.h"
#include "backend.h"

namespace vgraph {
namespace utils {
namespace logging {

class logger {
public:
    logger(const std::string& name);
    ~logger() = default;

    const void debug(const std::string& msg);
    const void info(const std::string& msg);
    const void warning(const std::string& msg);
    const void error(const std::string& msg);

private:
    const void log(ELogLevel level, const std::string& msg);

    const std::string name_;
    backend& backend_;
};

} // namespace logging
} // namespace utils
} // namespace vgraph

#endif // LOGGER_H