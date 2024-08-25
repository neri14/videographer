#include "manager.h"

#include "utils/logging/backend.h"
#include "utils/logging/stream_sink.h"

#include "telemetry/telemetry.h"

#include "video/generator.h"
#include "video/overlay/overlay.h"

#include <iostream>
#include <chrono>

namespace vgraph {

void manager::init(int argc, char* argv[])
{
    enable_logging();

    args = arguments::parse(argc, argv);
    set_log_level(args.debug ? utils::logging::ELogLevel::Debug : utils::logging::ELogLevel::Info);
}

void manager::run()
{
    log.info("Generation will use {}", args.gpu ? "GPU" : "CPU");
    if (!args.gpu) {
        log.warning("!! FOR BETTER PERFORMANCE CONSIDER GENERATING VIDEO ON GPU !! (-g/--gpu flag)");
    }

    std::shared_ptr<telemetry::telemetry> tele = telemetry::telemetry::load(args.telemetry, args.offset);

    auto t1 = std::chrono::high_resolution_clock::now();

    video::overlay::overlay overlay(args.resolution, args.timecode);
    overlay.precache();

    auto t2 = std::chrono::high_resolution_clock::now();

    video::generator gen(args.input, args.output, overlay, args.gpu, args.resolution, args.bitrate, args.debug);
    gen.generate();

    auto t3 = std::chrono::high_resolution_clock::now();

    log.info("Overlay pre-setup time: {:.3f} s",
             std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()/1000.0);
    log.info("Video generation time: {:.3f} s",
             std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count()/1000.0);
    log.info("Total time: {:.3f} s",
             std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t1).count()/1000.0);
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
