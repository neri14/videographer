#include "generator.h"
#include "pipeline.h"

extern "C" {
    #include <gst/gst.h>
}

namespace vgraph {
namespace video {

generator::generator(const std::string& input, const std::string& output):
    input_path_(input),
    output_path_(output)
{}

void generator::generate()
{
    log.info("Starting video generation");

    log.info("Initializing Gstreamer");
    gst_init(nullptr, nullptr);

    log.info("Creating pipeline");
    pipeline pipe(input_path_, output_path_);

    if (!pipe.build()) {
        log.error("Pipeline creation failed");
        return;
    }
    
    if (!pipe.link()) {
        log.error("Pipeline static linking failed");
        return;
    }

    if (!pipe.init()) {
        log.error("Pipeline initialization failed");
        return;
    }

    if (!pipe.run()) {
        log.error("Pipeline execution failed");
        return;
    }

    log.info("Finished video generation");
}

} // namespace video
} // namespace vgraph
