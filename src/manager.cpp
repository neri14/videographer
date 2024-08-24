#include "manager.h"

#include "utils/logging/backend.h"
#include "utils/logging/stream_sink.h"

#include "video/generator.h"
#include "video/overlay_drawer.h"

#include <iostream>

namespace vgraph {

void manager::init(int argc, char* argv[])
{
    enable_logging();

    args = arguments::parse(argc, argv);
    set_log_level(args.debug ? utils::logging::ELogLevel::Debug : utils::logging::ELogLevel::Info);
}

void manager::run()
{
    video::overlay_drawer overlay;
    video::generator gen(args.input, args.output, overlay);

    gen.generate();
}

void manager::enable_logging()
{
    log_sink = std::make_shared<utils::logging::stream_sink>(std::cout);
    utils::logging::backend::get_instance().add_sink(log_sink);
}

void manager::set_log_level(utils::logging::ELogLevel level)
{
    utils::logging::backend::get_instance().set_log_level(log_sink, level);
}

} // namespace vgraph
