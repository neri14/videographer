#ifndef MANAGER_H
#define MANAGER_H

#include "arguments.h"
#include "utils/logging/logger.h"
#include "utils/logging/sink.h"
#include "utils/logging/log_level.h"

namespace vgraph {

class manager {
public:
    manager() = default;
    void init(int argc, char* argv[]);

private:
    void enable_logging();
    void set_log_level(utils::logging::ELogLevel level);

    utils::logging::logger log{"manager"};

    std::shared_ptr<utils::logging::sink> log_sink;
    arguments args;

};

} // namespace vgraph

#endif // MANAGER_H