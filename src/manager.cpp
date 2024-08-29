#include "manager.h"

#include "utils/logging/backend.h"
#include "utils/logging/stream_sink.h"

#include "telemetry/telemetry.h"

#include "video/generator.h"
#include "video/overlay/layout.h"
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

    auto t1 = std::chrono::high_resolution_clock::now();

    std::shared_ptr<telemetry::telemetry> tele = nullptr;
    if (args.telemetry) {
        tele = telemetry::telemetry::load(*args.telemetry, args.offset);
        if (!tele) {
            log.error("Unable to load telemetry from defined path - exitting...");
            return;
        }
    } else {
        log.warning("NO TELEMETRY FILE PROVIDED - NO DATA-DRIVEN OVERLAY WILL BE GENERATED");
    }

    auto t2 = std::chrono::high_resolution_clock::now();

    video::overlay::layout_parser layout_parser;
    std::shared_ptr<video::overlay::layout> lay = nullptr;
    if (args.layout) {
        lay = layout_parser.parse(*args.layout);
        if (!lay) {
            log.error("Unable to load layout from defined path - exitting...");
            return;
        }
    } else {
        log.warning("NO LAYOUT FILE PROVIDED - NO DATA-DRIVEN OVERLAY WILL BE GENERATED");
    }

    video::overlay::overlay overlay(lay, tele, args.resolution, args.timecode);
    overlay.precache();

    auto t3 = std::chrono::high_resolution_clock::now();

    video::generator gen(args.input, args.output, overlay, args.gpu, args.resolution, args.bitrate, args.debug);
    gen.generate();

    auto t4 = std::chrono::high_resolution_clock::now();

    log.info("Telemetry processing time: {:.3f} s",
             std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()/1000.0);
    log.info("Overlay setup time: {:.3f} s",
             std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count()/1000.0);
    log.info("Video generation time: {:.3f} s",
             std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3).count()/1000.0);
    log.info("Total time: {:.3f} s",
             std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t1).count()/1000.0);
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
