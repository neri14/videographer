#include "manager.h"

#include "utils/logging/backend.h"
#include "utils/logging/stream_sink.h"

#include "telemetry/telemetry.h"

#include "video/generator.h"
#include "video/overlay/layout.h"
#include "video/overlay/layout_parser.h"
#include "video/overlay/overlay.h"
#include "video/overlay/widget/timecode_widget.h"

#include <iostream>
#include <chrono>
#include <memory>

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

    std::shared_ptr<video::overlay::layout> lay = nullptr;
    if (args.alignment_mode) {
        lay = video::overlay::generate_alignment_layout(tele, *args.resolution);
    } else if (args.layout) {
        video::overlay::layout_parser layout_parser;
        lay = layout_parser.parse(*args.layout);
        if (!lay) {
            log.error("Unable to load layout from defined path - exitting...");
            return;
        }
    } else {
        log.warning("NO LAYOUT FILE PROVIDED - NO DATA-DRIVEN OVERLAY WILL BE GENERATED");
    }

    if (args.timecode) {
        if (!lay) {
            lay = std::make_shared<video::overlay::layout>();
        }
        lay->push_back(std::make_shared<video::overlay::timecode_widget>(args.resolution->first / 2));
    }

    std::shared_ptr<video::overlay::overlay> overlay;
    if (tele != nullptr && lay != nullptr) {
        overlay = std::make_shared<video::overlay::overlay>(lay, tele, *args.resolution);
        overlay->precache();
    }

    auto t3 = std::chrono::high_resolution_clock::now();

    video::generator gen(*args.input, *args.output, overlay,
                         args.gpu, *args.resolution, *args.bitrate,
                         args.debug, args.clip_time);
    gen.generate();

    auto t4 = std::chrono::high_resolution_clock::now();

    log.info("Telemetry processing time: {:%H:%M:%S}",
             std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1));
    log.info("Overlay setup time: {:%H:%M:%S}",
             std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2));
    log.info("Video generation time: {:%H:%M:%S}",
             std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t3));
    log.info("Total time: {:%H:%M:%S}",
             std::chrono::duration_cast<std::chrono::milliseconds>(t4 - t1));
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
